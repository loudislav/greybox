<?php

namespace App;

use Illuminate\Auth\Authenticatable;
use Laravel\Lumen\Auth\Authorizable;
use Illuminate\Database\Eloquent\Model;
use Illuminate\Contracts\Auth\Authenticatable as AuthenticatableContract;
use Illuminate\Contracts\Auth\Access\Authorizable as AuthorizableContract;

class Registration extends Model implements AuthenticatableContract, AuthorizableContract
{
    use Authenticatable, Authorizable;

    /**
     * The attributes that are mass assignable.
     *
     * @var array
     */
    protected $fillable = [
        'person', 'note', 'event', 'role', 'accommodation', 'confirmed', 'team', 'registered_by'
    ];

    /**
     * The attributes excluded from the model's JSON form.
     *
     * @var array
     */
    protected $hidden = [
    ];

    /*
     * @return App\User
     */
    public function registeredBy()
    {
        return $this->belongsTo(User::class, 'registered_by', 'id');
    }

    public function person()
    {
        return $this->belongsTo(Person::class, 'person', 'id');
    }

    public function event()
    {
        return $this->belongsTo(Event::class, 'event', 'id');
    }

    public function role()
    {
        return $this->belongsTo(Role::class, 'role', 'id');
    }

    public function team()
    {
        return $this->belongsTo(Team::class, 'team', 'id');
    }

    private function getRegistrationGroupQuery()
    {
        $user = $this->registeredBy()->first();
        $event = $this->event()->first();
        return self::where([
            ['registered_by', '=', $user->id],
            ['event', '=', $event->id],
            ['confirmed', '=', false]
        ]);
    }

    public function getRegistrationGroup()
    {
        return $this->getRegistrationGroupQuery()->get();
    }

    public function getQuantifiedRoles()
    {
        $registrationGroupQuery = $this->getRegistrationGroupQuery();
        $quantifiedRoles = $registrationGroupQuery
            ->select('role', 'accommodation', \DB::raw('count(*) as quantity'))
            ->groupBy('role', 'accommodation')
            ->get();
        return $quantifiedRoles;
    }

    public function confirmRegistrationGroup($invoiceId = null)
    {
        $registrationGroupQuery = $this->getRegistrationGroupQuery();
        $registrationGroupQuery->update([
            'confirmed' => true,
            'invoice' => $invoiceId
        ]);
    }
}
