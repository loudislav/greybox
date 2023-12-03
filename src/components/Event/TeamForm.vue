<template>
  <!-- Submit on validation error is needed in order to inform user about errors -->
  <q-form class="team-form" @submit="submitForm" @validation-error="submitForm">
    <q-select
      outlined
      :model-value="teamName"
      :label="$tr('fields.teamName')"
      use-input
      hide-selected
      fill-input
      input-debounce="0"
      :options="teamsAutofill"
      @update:model-value="autofillSelected"
      @filter="filterTeamNames"
      @input-value="teamName = $event"
      lazy-rules
      :loading="loadingTeamFill"
      :rules="[$validators.nonEmpty]"
    >
      <template v-slot:option="scope">
        <q-item v-bind="scope.itemProps" v-on="scope.itemProps">
          <q-item-section>
            <q-item-label v-html="scope.opt.label" />
          </q-item-section>
          <q-item-section avatar>
            <q-avatar
              class="bg-negative text-white deletion-avatar deleting"
              size="30px"
              @click.stop="deleteTeam(scope.opt.value)"
            >
              <q-tooltip
                anchor="center left"
                self="center right"
                :offset="[10, 10]"
              >
                {{ $tr("autofill.removeTooltip.team") }}
              </q-tooltip>
              <q-icon name="fas fa-trash" />
            </q-avatar>
          </q-item-section>
        </q-item>
      </template>
    </q-select>

    <person-card
      :ref="`person_card_${id}`"
      v-for="(person, id, index) in people"
      v-bind:key="id"
      :visible="id == visibleId"
      :id="id"
      :index="index"
      :error="person.error"
      :autofill="person.autofill"
      @toggleVisibility="toggleVisibility"
      @delete="deletePerson"
      :possibleDiets="possibleDiets"
      :accommodationType="accommodationType"
      :mealType="mealType"
      :novices="novices"
      :requireEmail="requireEmail"
      :requireGender="requireGender"
    />
    <g-d-p-r-checkbox v-model="accept" :error="acceptError" />
    <div class="text-center">
      <q-btn
        color="blue-9"
        icon="fas fa-plus"
        :disabled="Object.keys(people).length >= maxMembers"
        :label="$tr('buttons.addDebater')"
        @click="addPerson"
        class="q-my-sm"
      />
      <q-btn
        :label="$tr('buttons.continue')"
        :disabled="!Object.keys(people).length"
        type="submit"
        color="primary"
        class="q-mx-sm q-my-sm"
      />
      <q-btn
        :label="$tr('buttons.clear')"
        type="reset"
        color="negative"
        @click="$emit('goToRolePick')"
        flat
        class="q-my-sm"
      />
    </div>
  </q-form>
</template>

<script>
/* eslint-disable */
import personCard from './TeamPersonCard';
import GDPRCheckbox from './GDPRCheckbox';

export default {
  name: 'TeamForm',
  components: {
    GDPRCheckbox,
    personCard,
  },
  props: {
    autofill: Object,
    accommodationType: String,
    mealType: String,
    novices: Boolean,
    possibleDiets: Array,
    eventId: Number,
    requireEmail: Boolean,
    requireGender: Boolean,
  },
  emits: [
    'goToRolePick',
    'submit',
    'autofillPerson',
  ],
  data() {
    return {
      pastTeams: [],
      teamsAutofill: [],
      translationPrefix: 'event.',
      people: {},
      visibleId: null,
      teamName: null,
      teamId: null,
      accept: false,
      acceptError: false,
      maxMembers: 5,
      loadingTeamFill: false,
    };
  },
  created() {
    this.addPerson();

    const cached = this.$db(`autofillTeams-event${this.eventId}`);

    if (cached) return (this.pastTeams = this.teamsAutofill = cached);

    this.$api({
      url: `user/${this.$auth.user().id}/team/event/${this.eventId}`,
      method: 'get',
    }).then((d) => {
      this.pastTeams = this.teamsAutofill = d.data;
      this.$db(`autofillTeams-event${this.eventId}`, this.pastTeams, true);
    });
  },

  computed: {
    // Teams in autofill already registered for this event
    registeredTeams() {
      return this.pastTeams.filter((item) => item.registered);
    },
    notRegisteredTeams() {
      return this.pastTeams.filter((item) => !item.registered);
    },
  },

  methods: {
    filterTeamNames(val, update) {
      if (val) this.teamId = null;

      update(() => {
        const needle = val.toLocaleLowerCase();
        this.teamsAutofill = this.notRegisteredTeams
          // Filter teams based on name
          .filter((v) => v.name.toLocaleLowerCase().includes(needle))
          // Parse teams to select compatible objects
          .map((item) => ({
            label: item.name,
            value: item.id,
          }));
      });
    },
    autofillSelected(value) {
      this.teamId = value.value;

      this.loadingTeamFill = true;

      // Team selected -> get its members
      this.$api({
        url: `team/${this.teamId}`,
        method: 'get',
      })
        .then((d) => {
          // Empty people
          this.people = {};
          this.addPerson();

          const data = d.data.members;

          // Autofill all members
          const autofillMember = (index) => {
            const member = data[index];

            this.$emit('autofillPerson', member);

            if (!data[index + 1]) {
              return this.$nextTick(() => {
                this.visibleId = null;
              });
            }

            this.$nextTick(() => {
              this.addPerson();

              this.$nextTick(() => {
                autofillMember(index + 1);
              });
            });
          };

          this.$nextTick(() => {
            autofillMember(0);
          });
        })
        .finally(() => {
          this.loadingTeamFill = false;
        });
    },
    toggleVisibility(id) {
      if (this.visibleId === id) this.visibleId = null;
      else this.visibleId = id;
    },
    deletePerson(id) {
      delete this.people[id];
    },
    addPerson() {
      if (Object.keys(this.people).length >= this.maxMembers) {
        return this.$flash(
          this.$tr('errors.teamLength', { len: this.maxMembers }),
          'error',
        );
      }

      let id;
      do {
        id = this.generateId();
      } while (this.people[id]);

      this.people[id] = {
        formData: null,
        autofillData: null,
        error: false,
        autofill: null,
      };
      this.toggleVisibility(id);
    },

    // Source: https://jsfiddle.net/yo39a9cw/
    generateId(len = 5) {
      let text = '';
      const chars = 'abcdefghijklmnopqrstuvwxyz0123456789';

      for (let i = 0; i < len; i++) {
        text += chars.charAt(Math.floor(Math.random() * chars.length));
      }

      return text;
    },

    submitForm() {
      const validationPromise = new Promise((resolve, reject) => {
        this.acceptError = !this.accept;
        if (!this.accept || !this.teamName || !this.teamName.trim().length) reject();

        const cards = Object.keys(this.people)
          .map((id) => this.$refs[`person_card_${id}`]);
        let validated = 0;
        let hasError = false;

        if (!cards.length) return reject();
        // Validate and submit all people
        cards.forEach(([item]) => {
          const formFields = item.$refs['form-fields'];
          const form = formFields.$refs['q-form'];
          const { id } = item;

          // Trigger qForm validation
          form.validate().then((isValid) => {
            validated++;
            if (isValid) {
              const values = formFields.sendForm();

              if (values) {
                this.people[id]['formData'] = values.formData;
                this.people[id]['autofillData'] = values.autofillData;
                this.people[id]['error'] = false;

                if (validated >= cards.length) {
                  if (hasError) reject();
                  else resolve();
                }

                return true;
              }
            }
            this.people[id]['error'] = true;
            hasError = true;

            if (validated >= cards.length) reject();
          });
        });
      });
      validationPromise
        .then(() => {
          this.$emit('submit', this.people, this.teamName, this.teamId);
        })
        .catch(() => {
          this.$flash(this.$tr('general.form.errors.form', null, false), 'error');
        });
    },
    deleteTeam(id) {
      this.$confirm({
        confirm: this.$tr('general.confirmModal.remove', null, false),
        message: this.$tr('autofill.removeModal.team.title'),
      }).onOk(() => {
        this.$bus.$emit('fullLoader', true);

        this.$api({
          url: 'deletedautofill',
          method: 'post',
          alerts: false,
          data: {
            team: id,
          },
        })
          .then(() => {
            // Remove removed person from cache
            this.pastTeams = this.pastTeams.filter((item) => item.id !== id);
            this.$db(
              `autofillTeams-event${this.eventId}`,
              this.pastTeams,
              true,
            );

            this.$flash(
              this.$tr('autofill.removeModal.team.success'),
              'success',
            );
          })
          .catch(() => {
            this.$flash(this.$tr('autofill.removeModal.team.error'), 'error');
          })
          .finally(() => {
            this.$bus.$emit('fullLoader', false);
          });
      });
    },
  },

  watch: {
    autofill(data) {
      if (this.visibleId && this.people[this.visibleId] && data) {
        this.people[this.visibleId].autofill = data;
      }
    },
  },
};
</script>
