<template>
  <div class="col-12 col-sm-6 col-md-4 col-lg-3 q-px-sm q-py-md items-stretch">
    <q-card class="thin-header-card normal-margin-card">
      <q-card-section class="bg-primary text-white card-header">
        <div class="row items-center no-wrap">
          <div class="col">
            <div class="text-h6">
              {{ person.person.name }} {{ person.person.surname }}
            </div>
          </div>

          <div class="col-auto" v-if="menu">
            <q-btn color="white" round flat icon="fas fa-ellipsis-v">
              <q-menu cover auto-close>
                <q-list class="smaller-margin-menu">
                  <q-item clickable @click="removePerson">
                    <q-item-section avatar>
                      <q-icon name="fas fa-trash" />
                    </q-item-section>
                    <q-item-section>{{
                      $tr("general.confirmModal.remove", null, false)
                    }}</q-item-section>
                  </q-item>
                </q-list>
              </q-menu>
            </q-btn>
          </div>
        </div>
      </q-card-section>

      <q-card-section>
        <div v-if="person.team">
          <dt>{{ $tr("types.team") }}:</dt>
          <dd>{{ person.team.name }}</dd>
        </div>

        <div
          v-for="(value, fieldName) in registration"
          v-bind:key="'registration-fields-' + fieldName"
        >
          <template
            v-if="
              showRegistrationFields.includes(fieldName) &&
                (fieldName !== 'teamName' || value) &&
                (fieldName !== 'role' || roleName)
            "
          >
            <dt>{{ $tr("registrationFields." + fieldName) }}:</dt>

            <dd>
              <template v-if="fieldName === 'role'">
                {{ roleName }}
              </template>
              <template v-else-if="fieldName === 'accommodation'">
                {{
                  value ? $tr("checkout.values.yes") : $tr("checkout.values.no")
                }}
              </template>
              <template v-else>
                {{ value }}
              </template>
            </dd>
          </template>
        </div>

        <div v-if="person.person.speaker_status">
          <dt>{{ $tr("fields.speakerStatus") }}:</dt>
          <dd>{{ person.person.speaker_status.toUpperCase() }}</dd>
        </div>

        <div>
          <dt>{{ $tr("registrationFields.meals") }}:</dt>
          <dd>
            {{
              registration.meals
                ? $tr("checkout.values.yes")
                : $tr("checkout.values.no")
            }}
          </dd>
        </div>

        <div v-if="novices">
          <dt>{{ $tr("fields.novice") }}:</dt>
          <dd>
            {{
              registration.novice
                ? $tr("checkout.values.yes")
                : $tr("checkout.values.no")
            }}
          </dd>
        </div>

        <template v-if="!dietaryRequirement">{{ dietaryRequirement }}</template>
        <div
          v-if="registration.meals && person.person.dietary_requirement && dietaryRequirement"
        >
          <dt>{{ $tr("fields.diet") }}:</dt>
          <dd>{{ dietaryRequirement }}</dd>
        </div>

        <div v-if="person.person.email && person.person.email">
          <dt>{{ $tr("auth.fields.email", null, false) }}:</dt>
          <dd>{{ person.person.email }}</dd>
        </div>
      </q-card-section>

      <q-separator v-if="registration.accommodation && person.person.city" inset />

      <q-card-section v-if="registration.accommodation && person.person.city">
        <div
          v-for="(value, fieldName) in person.person"
          v-bind:key="'person-fields-' + fieldName"
        >
          <template
            v-if="
              value &&
                fieldName.substr(-4) !== 'name' &&
                !ignorePersonFields.includes(fieldName)
            "
          >
            <dt>{{ $tr("fields." + fieldName) }}:</dt>
            <dd v-if="fieldName === 'birthdate'">
              {{ getDate(value, "D. M. YYYY") }}
              <!--
              {{ /*(value).format("D. M. Y")*/ }}
            --></dd>
            <dd v-else-if="fieldName === 'gender'">
              {{ $tr('fields.genderValues.' + value)}}
            </dd>
            <dd v-else-if="value != null">
              {{ value }}
            </dd>
          </template>
        </div>
      </q-card-section>
    </q-card>
  </div>
</template>

<script>
import { date } from 'quasar';
import { mapGetters } from 'vuex';

/* eslint-disable */
export default {
  props: {
    person: Object,
    registration: Object,
    personIndex: Number,
    possibleDiets: {
      type: Array,
      required: false,
      default: [],
    },
    novices: Boolean,
    menu: {
      type: Boolean,
      required: false,
      default: true,
    },
  },
  emits: ['remove'],
  data() {
    return {
      translationPrefix: 'event.',
      roles: {},
      showRegistrationFields: ['teamName', 'accommodation', 'role'],
      ignorePersonFields: ['dietary_requirement', 'email', 'id', 'created_at', 'updated_at', 'old_greybox_id', 'school_year', 'parent_email'],
    };
  },
  computed: {
    ...mapGetters('roles', [
      'role',
    ]),
    roleName() {

      if (this.person.role) {
        return this.$tr(this.person.role.name);
      }

      const role = this.role(this.person.registration?.role);
      if (!role) {
        return null;
      }

      return this.$tr(role.name);
    },
    dietaryRequirement() {
      const id = this.person.person.dietary_requirement;

      const diet = this.possibleDiets.find((item) => item.id === id);

      if (!diet) {
        return null;
      }

      // Get dietary requirement name
      const name = this.$tr(diet.name,);

      // Capitalize
      return name.charAt(0).toUpperCase() + name.slice(1);
    },
  },
  name: 'CheckoutPersonCard',
  methods: {
    removePerson() {
      this.$confirm({
        confirm: this.$tr('general.confirmModal.remove', null, false),
        message: this.$tr('checkout.actions.removeModal.title'),
      }).onOk(() => {
        this.$emit('remove', this.personIndex);
      });
    },
    getDate: date.formatDate,
  },
};
</script>
