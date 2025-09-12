<?php

namespace App;

use Illuminate\Auth\Authenticatable;
use Laravel\Lumen\Auth\Authorizable;
use Illuminate\Database\Eloquent\Model;
use Illuminate\Contracts\Auth\Authenticatable as AuthenticatableContract;
use Illuminate\Contracts\Auth\Access\Authorizable as AuthorizableContract;

class Event extends Model implements AuthenticatableContract, AuthorizableContract
{
    use Authenticatable, Authorizable;

    /**
     * The attributes that are mass assignable.
     *
     * @var array
     */
    protected $fillable = [
        'organizer',
        'competition',
        'finals',
        'name',
        'beginning',
        'end',
        'place',
        'accommodation',
        'meals',
        'novices',
        'membership_required',
        'email_required',
        'parent_email_required',
        'gender_required',
        'soft_deadline',
        'hard_deadline',
        'invoice_text',
        'note',
        'reply_email'
    ];

    /**
     * The attributes excluded from the model's JSON form.
     *
     * @var array
     */
    protected $hidden = [
    ];

    public function prices()
    {
        return $this->hasMany(Price::class, 'event', 'id');
    }

    public function registrations()
    {
        return $this->hasMany(Registration::class, 'event', 'id');
    }

    public function nameTranslation()
    {
        return $this->belongsTo(Translation::class, 'name', 'id');
    }

    public function invoiceTextTranslation()
    {
        return $this->belongsTo(Translation::class, 'invoice_text', 'id');
    }

    public function noteTranslation()
    {
        return $this->belongsTo(Translation::class, 'note', 'id');
    }

    public function dietaryRequirements()
    {
        return $this->belongsToMany('App\DietaryRequirement', 'events_dietary_requirements', 'event', 'dietary_requirement');
    }

    public function getDiscountTime()
    {
        $softDeadlineDate = substr($this->soft_deadline, 0, 10);
        $softDeadlineMidnight = $softDeadlineDate . ' 23:59:59';
        return strtotime($softDeadlineMidnight . " -7 days");
    }

    public function isDiscountAvailable()
    {
        if (time() <= $this->getDiscountTime()) {
            return true;
        }
        return false;
    }

    /**
     * Returns true if the tournament hasn't ended yet.
     * @return bool
     */
    public function isCurrent(): bool
    {
        return time() < strtotime($this->end);
    }

    /**
     * Returns Event with name Translation
     * @return Event
     */
    public function withName(): Event
    {
        $this->name = $this->nameTranslation()->first();
        return $this;
    }

    /**
     * Returns all organizers of the given event.
     * @return Person[]
     */
    public function organizers(): array
    {
        $registrations = $this->registrations()->where('role', env('ORGANIZER_ROLE_ID'))->get();
        $organizers = [];
        foreach ($registrations as $registration) {
            $organizers[] = $registration->person()->first();
        }
        return $organizers;
    }

    public function isPds(): bool
    {
        if ('pds' === $this->organizer) return true;
        return false;
    }

    public function isEurosdc(): bool
    {
        if ('eurosdc' === $this->organizer) return true;
        return false;
    }
}
