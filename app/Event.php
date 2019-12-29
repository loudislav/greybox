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
        'name', 'beginning', 'end', 'place', 'soft_deadline', 'hard_deadline', 'invoice_text', 'note'
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
        return $this->hasMany(Registration::class, 'event_id', 'id');
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
}