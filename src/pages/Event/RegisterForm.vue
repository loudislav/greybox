<template>
  <div class="row q-col-gutter-md reverse" v-if="event && role">
    <div class="col-12 col-sm-4 col-md-5 autofill-wrapper">
      <autofill-card @person-selected="personSelected" :eventId="eventId" />
    </div>
    <div class="col-12 col-sm-8 col-md-7">
      <form-fields
        v-if="role.id !== Infinity"
        @submit="sendForm"
        :autofill="autofillData"
        :accommodationType="event.accommodation"
        :mealType="event.meals"
        :novices="event.novices"
        :possibleDiets="event.dietaryRequirements"
        :role="role.id"
        :requireEmail="event.email_required"
        :requireGender="event.gender_required"
        :requireParentEmail="event.parent_email_required"
        @goToRolePick="goToRolePick()"
      />
      <team-form
        v-else
        @submit="submitTeamForm"
        @goToRolePick="goToRolePick()"
        @autofillPerson="personSelected"
        :autofill="autofillData"
        :accommodationType="event.accommodation"
        :mealType="event.meals"
        :novices="event.novices"
        :possibleDiets="event.dietaryRequirements"
        :eventId="eventId"
        :requireEmail="event.email_required"
        :requireGender="event.gender_required"
        :requireParentEmail="event.parent_email_required"
      ></team-form>
    </div>
    <!-- Success -->
    <q-dialog v-model="showGroupModal" persistent>
      <q-card>
        <q-card-section class="row items-center">
          <div class="text-h6">{{ $tr('groupModal.title') }}</div>
        </q-card-section>
        <q-card-section class="row items-center text-center">
          <q-avatar
            icon="fas fa-check"
            class="margin-center"
            color="primary"
            text-color="white"
          />
        </q-card-section>

        <q-card-actions align="right">
          <q-btn
            flat
            :label="$tr('groupModal.anotherPerson')"
            color="primary"
            v-close-popup
            :to="rolePickRoute"
          />
          <q-btn
            flat
            :label="$tr('groupModal.submit')"
            color="primary"
            v-close-popup
            :to="checkoutRoute"
          />
        </q-card-actions>
      </q-card>
    </q-dialog>
  </div>
</template>

<script lang="ts">
/* eslint-disable */
// @ts-nocheck
import { defineComponent } from 'vue';
import { Role } from 'src/types/role';
import { Event, EventFull } from 'src/types/event';
import autofillCard from 'components/Event/AutofillCard.vue';
import formFields from 'components/Event/FormFields.vue';
import teamForm from 'components/Event/TeamForm.vue';
import { translationMatchesInAnyLanguage } from 'boot/i18n';
import config from 'src/config';

export default defineComponent({
  name: 'RegisterForm',

  props: {
    eventId: {
      type: Number,
      required: true,
    },
  },

  data() {
    return {
      translationPrefix: 'event.',
      autofillData: null,
      showGroupModal: false,
    };
  },

  components: {
    autofillCard,
    formFields,
    teamForm,
  },

  computed: {
    role(): Role {
      // eslint-disable-next-line max-len
      // eslint-disable-next-line @typescript-eslint/no-unsafe-call,@typescript-eslint/no-unsafe-member-access
      return <Role> this.$store.getters['roles/roleFromSlug'](
        this.$route.params.role,
      );
    },
    event(): EventFull {
      return <Event> this.$store.getters['events/fullEvent'](this.eventId);
    },
    rolePickRoute() {
      return this.$translatedRouteLink({
        name: 'event-pick-role',
        params: {
          ...this.$route.params,
          type: this.$tr('paths.eventParams.type.group', null, false),
        },
      });
    },
    checkoutRoute() {
      return this.$translatedRouteLink({
        name: 'event-checkout',
        params: {
          ...this.$route.params,
          checkout: this.$tr('paths.eventParams.checkout', null, false),
        },
      });
    }
  },

  methods: {
    personSelected(data) {
      this.autofillData = data;
    },

    sendForm(data, autofill) {
      const personData = data;

      const registrationData = {
        person: null,
        event: this.eventId,
        role: this.role.id === Infinity ? config.debaterRoleId : this.role.id, // if role is team, set as debater
        accommodation: data.accommodation,
        meals: data.meals,
        novice: data.novice,
        team: data.team || null,
        teamName: data.teamName || null,
        note: data.note,
      };

      // Move data from person to registration
      delete personData.team;
      delete personData.teamName;
      delete personData.accommodation;
      delete personData.meals;
      delete personData.note;

      this.$store.commit('eventRegistrationForm/addEventRegistration', [this.eventId, {
        person: personData,
        registration: registrationData,
        autofill,
      }]);

      // Autofilled person -> remove from autofill
      if (autofill) {
        this.$db(
          `autofillDebaters-event${this.eventId}`,
          this.$db(`autofillDebaters-event${this.eventId}`)
            .map((item) => ({
              ...item,
              registered: item.id === autofill.id ? true : item.registered,
            })),
          true,
        );
      }

      if (translationMatchesInAnyLanguage('paths.eventParams.type.individual', this.$route.params.type)) {
        this.$router.push(this.checkoutRoute);
      } else {
        this.showGroupModal = true;
      }
    },

    submitTeamForm(people, teamName, teamId) {
      // Function to call after team ID is known
      const doneCallback = (id, name) => {
        for (const index in people) {
          const person = people[index];

          person.formData.team = id;
          person.formData.teamName = name;

          this.sendForm(person.formData, person.autofillData);
        }
      };

      // Team is autofilled -> call callback right away
      if (teamId) return doneCallback(teamId, teamName);

      // Team is new -> submit to API first before we can know the ID
      this.$bus.$emit('fullLoader', true);
      this.$api({
        url: 'team',
        data: {
          name: teamName,
          event: this.event.id,
        },
        alerts: false
      })
        .then((data) => {
          doneCallback(data.data.id, teamName);
        })
        .catch(({response}) => {
          if (
            response
            && response.status === 422
            && response.data.name[0] === 'validation.max.string'
          ) {
            this.$flash(this.$tr('validation.teamNameTooLong', null, false), 'error');
          } else {
            this.$flash(this.$tr('general.form.error', null, false), 'error');
          }
        })
        .finally(() => {
          this.$bus.$emit('fullLoader', false);
        });
    },

    goToRolePick(): void {
      this.$router.push(this.rolePickRoute);
    },
  },
});
</script>
