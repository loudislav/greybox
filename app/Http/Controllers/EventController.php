<?php


namespace App\Http\Controllers;

use App\Event,
    App\Translation,
    App\Events\EventCreated,
    App\Services\TeamRulesCheckingService;
use Illuminate\Http\Request,
    Illuminate\Support\Facades\Event as EventFacade;

class EventController extends Controller
{
    public function __construct()
    {
        $this->middleware('auth', ['only' => [
            'showOne',
            'create',
            'update',
            'delete',
            'showRegistrations',
            'showTeams'
        ]]);
    }

    public function showAll()
    {
        $user = \Auth::user();
        if ($user !== null && $user->can('showAll', Event::class)) {
            $events = Event::all();
        } else {
            $events = Event::where('end', '>=', date("Y-m-d"))->get();
        }
        foreach ($events as $event) {
            $event->name = $event->nameTranslation()->first();
            $event->invoice_text = $event->invoiceTextTranslation()->first();
            $event->note = $event->noteTranslation()->first();
        }
        return response()->json($events);
    }

    public function showOne($id)
    {
        $event = Event::find($id);

        if (null === $event) {
            return response()->json(['message' => 'eventNotFound'], 404);
        }

        $event->name = $event->nameTranslation()->first();
        $event->invoice_text = $event->invoiceTextTranslation()->first();
        $event->note = $event->noteTranslation()->first();

        $event->prices = $event->prices()->get();
        for ($i=0; $i<count($event->prices); $i++) {
            $priceDescription = $event->prices[$i]->translation()->first();
            if ('Accommodation' == $priceDescription->en) {
                unset($event->prices[$i]);
            } else {
                $event->prices[$i]->role = $event->prices[$i]->role()->first();
            }
        }

        $dietaryRequirements = $event->dietaryRequirements()->orderBy('order')->get();
        foreach ($dietaryRequirements as $dietaryRequirement)
        {
            $dietaryRequirement->name = $dietaryRequirement->translation()->first();
        }
        $event->dietaryRequirements = $dietaryRequirements;

        return response()->json($event);
    }

    /*
     * TODO: Add accommodation, meals and membership required
     */
    public function create(Request $request)
    {
        $this->validate($request, [
            'name_cs' => 'required',
            'beginning' => 'required',
            'end' => 'required',
            'place' => 'required',
            'soft_deadline' => 'required',
            'hard_deadline' => 'required'
        ]);

        $this->authorize('create', Event::class);

        try {
            $nameTranslation = Translation::create([
                'cs' => $request->input('name_cs'),
                'en' => $request->input('name_en')
            ]);
            $invoiceTextTranslation = new Translation();
            if ($request->has('invoice_cs')) {
                $invoiceTextTranslation = Translation::create([
                    'cs' => $request->input('invoice_cs'),
                    'en' => $request->input('invoice_en')
                ]);
            }
            $noteTranslation = new Translation();
            if ($request->has('note_cs')) {
                $noteTranslation = Translation::create([
                    'cs' => $request->input('note_cs'),
                    'en' => $request->input('note_en')
                ]);
            }

            $event = Event::create([
                'organizer' => $request->input('organizer', 'adk'),
                'competition' => $request->input('competition'),
                'finals' => $request->input('finals', 0),
                'name' => $nameTranslation->id,
                'beginning' => $request->input('beginning'),
                'end' => $request->input('end'),
                'place' => $request->input('place'),
                'accommodation' => $request->input('accommodation', 'opt-out'),
                'meals' => $request->input('meals', 'opt-out'),
                'novices' => $request->input('novices', 0),
                'membership_required' => $request->input('membership_required', 1),
                'email_required' => $request->input('email_required', 0),
                'parent_email_required' => $request->input('parent_email_required', 0),
                'gender_required' => $request->input('gender_required', 0),
                'soft_deadline' => $request->input('soft_deadline'),
                'hard_deadline' => $request->input('hard_deadline'),
                'invoice_text' => $invoiceTextTranslation->id,
                'note' => $noteTranslation->id,
                'reply_email' => $request->input('reply_email')
            ]);
            $event->dietaryRequirements()->attach($request->input('dietary_requirements'));

            EventFacade::dispatch(new EventCreated($event));

            $event->name = $nameTranslation;
            $event->invoice_text = $invoiceTextTranslation;
            $event->note = $noteTranslation;
            return response()->json($event, 201);
        } catch (\Illuminate\Database\QueryException $e) {
            return response()->json(['message' => $e->getMessage()], 500);
        }
    }

    public function update($id, Request $request)
    {
        try {
            $event = Event::findOrFail($id);
            $this->authorize('update', $event);

            if ($request->has('organizer')) $this->updateColumn($event, 'organizer', $request->input('organizer'));
            if ($request->has('competition')) $this->updateColumn($event, 'competition', $request->input('competition'));
            if ($request->has('finals')) $this->updateColumn($event, 'finals', $request->input('finals'));
            if ($request->has('name_cs')) {
                $nameTranslation = $event->nameTranslation()->updateOrCreate([], [
                    'cs' => $request->input('name_cs'),
                    'en' => $request->input('name_en')
                ]);
                $this->updateColumn($event, 'name', $nameTranslation->id);
            }
            if ($request->has('beginning')) $this->updateColumn($event, 'beginning', $request->input('beginning'));
            if ($request->has('end')) $this->updateColumn($event, 'end', $request->input('end'));
            if ($request->has('place')) $this->updateColumn($event, 'place', $request->input('place'));
            if ($request->has('accommodation')) $this->updateColumn($event, 'accommodation', $request->input('accommodation'));
            if ($request->has('meals')) $this->updateColumn($event, 'meals', $request->input('meals'));
            if ($request->has('novices')) $this->updateColumn($event, 'novices', $request->input('novices'));
            if ($request->has('membership_required')) $this->updateColumn($event, 'membership_required', $request->input('membership_required'));
            if ($request->has('email_required')) $this->updateColumn($event, 'email_required', $request->input('email_required'));
            if ($request->has('parent_email_required')) $this->updateColumn($event, 'parent_email_required', $request->input('parent_email_required'));
            if ($request->has('gender_required')) $this->updateColumn($event, 'gender_required', $request->input('gender_required'));
            if ($request->has('soft_deadline')) $this->updateColumn($event, 'soft_deadline', $request->input('soft_deadline'));
            if ($request->has('hard_deadline')) $this->updateColumn($event, 'hard_deadline', $request->input('hard_deadline'));
            if ($request->has('invoice_cs')) {
                $invoiceTextTranslation = $event->invoiceTextTranslation()->updateOrCreate([], [
                    'cs' => $request->input('invoice_cs'),
                    'en' => $request->input('invoice_en')
                ]);
                $this->updateColumn($event, 'invoice_text', $invoiceTextTranslation->id);
            }
            if ($request->has('note_cs')) {
                $noteTranslation = $event->noteTranslation()->updateOrCreate([],[
                    'cs' => $request->input('note_cs'),
                    'en' => $request->input('note_en')
                ]);
                $this->updateColumn($event, 'note', $noteTranslation->id);
            }
            if ($request->has('reply_email')) $this->updateColumn($event, 'reply_email', $request->input('reply_email'));
            if ($request->has('dietary_requirements')) {
                $event->dietaryRequirements()->sync($request->input('dietary_requirements'));
            }

            $event->name = $event->nameTranslation()->first();
            $event->invoice_text = $event->invoiceTextTranslation()->first();
            $event->note = $event->noteTranslation()->first();
            return response()->json($event, 200);
        } catch (\Illuminate\Database\QueryException $e) {
            return response()->json(['message' => $e->getMessage()], 500);
        }
    }

    public function delete($id)
    {
        $this->authorize('delete', Event::class);

        try {
            $event = Event::findOrFail($id);
            $event->dietaryRequirements()->detach();
            $event->delete();
            $nameTranslation = $event->nameTranslation()->first();
            if (null != $nameTranslation) $nameTranslation->delete();
            $invoiceTextTranslation = $event->invoiceTextTranslation()->first();
            if (null != $invoiceTextTranslation) $invoiceTextTranslation->delete();
            $noteTranslation = $event->noteTranslation()->first();
            if (null != $noteTranslation) $noteTranslation->delete();
            return response()->json(['message' => 'Deleted successfully.'], 204);
        } catch (\Illuminate\Database\QueryException $e) {
            return response()->json(['message' => $e->getMessage()], 500);
        }
    }

    public function showRegistrations($id)
    {
        $event = Event::find($id);
        $this->authorize('showRegistrations', $event);

        $registrations = $event->registrations()->get();

        foreach ($registrations as $registration) {
            $registration->role = $registration->role()->first();
            $registration->role->name = $registration->role->translation()->first();
            $registration->person = $registration->person()->first();
            $registration->person->dietary_requirement = $registration->person->dietaryRequirement()->first();
            if (!empty($registration->person->dietary_requirement)) $registration->person->dietary_requirement->name = $registration->person->dietary_requirement->translation()->first();
            $registration->team = $registration->team()->first();
        }

        return response()->json($registrations);
    }

    public function showTeams($id)
    {
        $event = Event::find($id);
        $this->authorize('showRegistrations', $event);

        $registrations = $event->registrations()->select('team')->distinct()->whereNotNull('team')->get();
        $teams = array();
        foreach ($registrations as $registration)
        {
            $teams[] = $registration->team()->first();
        }
        usort($teams, function($a, $b)
        {
            return collator_compare(new \Collator('cs_CZ'), $a->name, $b->name);
        });

        return response()->json($teams);
    }

    public function showTeamsDetails($id)
    {
        $event = Event::find($id);
        $this->authorize('showRegistrations', $event);

        $rulesCheckingService = new TeamRulesCheckingService();
        $registrations = $event->registrations()->where('role', 1)->get();

        $teams = array();
        foreach ($registrations as $registration)
        {
            $team = $registration->team()->first();
            if (null === $team)
            {
                continue;
            }
            $id = $team->id;
            if (!array_key_exists($id, $teams)) $teams[$id] = new \stdClass();
            $teams[$id]->team = $team;
            $teams[$id]->members[] = $registration->person()->first();
            $teams[$id]->registered_by = $team->registeredBy()->first()->withPerson();
        }

        $teamsToPublish = array();
        foreach ($teams as $team)
        {
            $team->warnings = $rulesCheckingService->checkTeamRules($team->team->name, $team->members, $event->competition, $event->finals);
            $teamsToPublish[] = $team;
        }

        return response()->json($teamsToPublish);
    }

    public function showUserRegistrations($eventId, $userId)
    {
        $event = Event::findOrFail($eventId);
        $user = \App\User::findOrFail($userId);
        try {
            if ($user->id !== \Auth::user()->id) {
                $this->authorize('showUserRegistrations', Event::class);
            }
        } catch (\Exception $exception) {
            return response()->json(['message' => 'user not logged in'], 401);
        }

        $registrations = $event->registrations()->where('registered_by', $user->id)->get();

        foreach ($registrations as $registration) {
            $registration->role = $registration->role()->first();
            $registration->role->name = $registration->role->translation()->first();
            $registration->person = $registration->person()->first();
            $registration->person->dietary_requirement = $registration->person->dietaryRequirement()->first();
            if (!empty($registration->person->dietary_requirement)) $registration->person->dietary_requirement->name = $registration->person->dietary_requirement->translation()->first();
            $registration->team = $registration->team()->first();
        }

        return response()->json($registrations);
    }

    public function draw($eventId, $roundNumber)
    {
        $matchingProcessor = new \DebateMatch\MatchingProcessor();
    }
}