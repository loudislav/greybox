<?php

namespace App;

use Illuminate\Auth\Authenticatable;
use Laravel\Lumen\Auth\Authorizable;
use Illuminate\Database\Eloquent\Model;
use Illuminate\Contracts\Auth\Authenticatable as AuthenticatableContract;
use Illuminate\Contracts\Auth\Access\Authorizable as AuthorizableContract;

class Person extends Model implements AuthenticatableContract, AuthorizableContract
{
    use Authenticatable, Authorizable;

    /**
     * The attributes that are mass assignable.
     *
     * @var array
     */
    protected $fillable = [
        'name', 'surname', 'email', 'institution', 'school_year', 'gender', 'birthdate', 'id_number', 'street', 'city', 'zip', 'parent_email', 'dietary_requirement', 'speaker_status', 'school', 'note'
    ];

    /**
     * The attributes excluded from the model's JSON form.
     *
     * @var array
     */
    protected $hidden = [
    ];

    public function user()
    {
        return $this->hasOne('App\User', 'person', 'id');
    }

    public function membership()
    {
        // TODO: to be changed to hasMany maybe - in case of repeated membership
        return $this->hasOne('App\Membership', 'person', 'id');
    }

    public function institution()
    {
        return $this->belongsTo(Institution::class, 'institution', 'id');
    }

    public function roles()
    {
        return $this->belongsToMany(Role::class);
    }

    public function registrations()
    {
        return $this->hasMany(Registration::class, 'person', 'id');
    }

    public function dietaryRequirement()
    {
        return $this->belongsTo(DietaryRequirement::class, 'dietary_requirement', 'id');
    }

    /**
     * @return Institution
     */
    public function isAHeadOf(): Institution
    {
        return $this->hasOne(Institution::class, 'head', 'id');
    }

    /**
     * @return string
     */
    public function getOldGreyboxId()
    {
        return $this->old_greybox_id;
    }

    /**
     * True if Adjudicator is among Roles
     * @return bool
     */
    public function isAdjudicator(): bool
    {
        return $this->roles()->where('id','2')->exists(); // 2 = ID of Adjudicator Role
    }

    /**
     * @return string | null
     */
    public function getWrappedUrl(): ?string
    {
        if (null === $this->old_greybox_id) return null;

        // Convert the integer to a string.
        $integer_str = strval($this->old_greybox_id);

        // Concatenate the integer string and the salt.
        $str_with_salt = $integer_str . '$salt';

        // Hash the concatenated string using the SHA256 algorithm.
        $hashed_str = hash('sha256', $str_with_salt);

        // Concatenate the URL and the hashed string
        $url = 'https://adk-wrapped.fly.dev/link/' . $hashed_str;

        return $url;
    }
}