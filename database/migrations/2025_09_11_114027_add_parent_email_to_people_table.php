<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

return new class extends Migration
{
    /**
     * Run the migrations.
     *
     * @return void
     */
    public function up()
    {
        Schema::table('people', function (Blueprint $table) {
            $table->string('parent_email', 63)->nullable()->after('zip');
        });

        Schema::table('events', function (Blueprint $table) {
            $table->boolean('parent_email_required')->after('email_required')->default(0);
        });
    }

    /**
     * Reverse the migrations.
     *
     * @return void
     */
    public function down()
    {
        Schema::table('events', function (Blueprint $table) {
            $table->dropColumn('parent_email_required');
        });

        Schema::table('people', function (Blueprint $table) {
            $table->dropColumn('parent_email');
        });
    }
};
