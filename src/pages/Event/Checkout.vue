<template>
  <div class="q-px-lg">
    <div class="row q-mb-md justify-center">
      <div class="col-12 col-sm-6 col-md-3 q-py-xs items-stretch">
        <q-card class="thin-header-card normal-margin-card">
          <q-card-section class="bg-blue-9 text-white card-header">
            <div class="row items-center no-wrap">
              <div class="col">
                <div class="text-h6">{{ $tr('billing.title') }}</div>
              </div>
              <div class="col-auto">
                <billing-menu @selected="changeBilling" />
              </div>
            </div>
          </q-card-section>

          <q-card-section>
            <div v-if="!billingClient">
              {{ $auth.user().username }}
            </div>
            <div v-else>
              <p class="q-mb-sm">
                <b>{{ billingClient.name }}</b>
                <template v-if="billingClient.email">
                  <br />{{ billingClient.email }}
                </template>
              </p>
              <p class="q-mb-sm" v-if="billingClient.registration_no">
                {{ $tr('billing.fields.registration_no') }}:
                {{ billingClient.registration_no }}
              </p>
              <p
                class="q-mb-none"
                v-if="
                  billingClient.street ||
                    billingClient.city ||
                    billingClient.zip ||
                    selectedCountry
                "
              >
                {{ billingClient.street }} <br v-if="billingClient.street" />
                {{
                  billingClient.city
                }}
                <template v-if="billingClient.zip && billingClient.city"
                >,
                </template>
                {{ billingClient.zip }}
                <template v-if="selectedCountry">
                  <br v-if="billingClient.city || billingClient.zip" />
                  {{ $tr(selectedCountry) }}

                  <!-- Element has to be mounted in order for API to be loaded -->
                  <country-select v-show="false" />
                </template>
              </p>
            </div>
          </q-card-section>
        </q-card>
      </div>
    </div>

    <div class="row -q-mx-sm justify-center">
      <person-card
        v-for="(person, index) in formData"
        v-bind:key="JSON.stringify(person)"
        :person="person"
        :registration="person.registration"
        :person-index="index"
        :possible-diets="possibleDiets"
        :novices="event.novices"
        @remove="removePerson"
      />
    </div>

    <div class="q-pt-md row">
      <div class="col-12 col-sm-6">
        <q-btn
          :label="$tr('back')"
          color="blue-9"
          class="q-my-xs"
          :to="rolePickRoute"
        />
      </div>
      <div class="col-12 col-sm-6">
        <q-btn
          :loading="loading"
          class="float-right q-my-xs"
          size="lg"
          color="primary"
          @click="sendForm"
        >
          {{ $tr('submit') }}
          <template v-slot:loading>
            <q-spinner-hourglass class="on-left" />
            {{ $tr('loading') }}
          </template>
        </q-btn>
      </div>
    </div>
  </div>
</template>

<script lang="ts">
/* eslint-disable */
// @ts-nocheck
import { defineComponent } from 'vue';
import PersonCard from 'src/components/Event/CheckoutPersonCard.vue';
import billingMenu from 'src/components/Event/BillingMenu.vue';
import CountrySelect from 'src/components/Event/CountrySelect.vue';

export default defineComponent({
  name: 'Checkout',
  props: {
    eventId: {
      type: Number,
      required: true,
    },
  },
  emits: [
    'confirm',
  ],
  data() {
    return {
      translationPrefix: 'event.checkout.',
      loading: false,
      billingClient: null,
    };
  },
  computed: {
    selectedCountry() {
      if (this.$organizer === 'adk' || !this.billingClient || !this.billingClient.country) return null;

      const db = this.$db('countries-select');

      if (!db) return null;

      const filtered = db.filter(
        (item) => item.value === this.billingClient.country,
      );

      if (filtered.length) return filtered[0].label;

      return null;
    },
    formData() {
      return this.$store.state.eventRegistrationForm[this.eventId]?.dataToSubmit ?? [];
    },
    event() {
      return this.$store.getters['events/fullEvent'](this.eventId);
    },
    possibleDiets() {
      return this.event.dietaryRequirements;
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
  },
  beforeMount() {
    if (!this.formData.length) {
      this.$router.replace(this.rolePickRoute);
    }
  },
  methods: {
    sendForm() {
      this.loading = !this.loading;

      // Send person and registration requests
      const registrationPromise = new Promise((resolve, reject) => {
        const registerCount = this.formData.length;
        let registered = 0;

        for (const index in this.formData) {
          const person = this.formData[index];
          if (person.person.dietary_requirement === null) {
            this.$store.commit('eventRegistrationForm/removeDietaryRequirements', [this.eventId, index]);
          }

          const createPerson = this._createPerson(person);

          createPerson
            .then((person_id) => {
              this.$store.commit('eventRegistrationForm/setRegistrationPersonId', [this.eventId, {
                registrationIndex: index,
                personId: person_id,
              }]);

              if (person.registered_data) {
                registered++;
                if (registerCount <= registered) return resolve(person.registered_data);
              }

              this.$api({
                url: 'registration',
                data: person.registration,
                method: 'post',
                alerts: false,
              })
                .then((data) => {
                  registered++;
                  this.$store.commit('eventRegistrationForm/setRegisteredData', [this.eventId, {
                    registrationIndex: index,
                    data,
                  }]);
                  if (registerCount <= registered) return resolve(data);
                })
                .catch((data) => {
                  reject([data, person]);
                });
            })
            .catch((data) => {
              reject([data, person]);
            });
        }
      });

      // All done -> ask for confirmation
      registrationPromise
        .then((data) => {
          const confirmData = {
            lang: this.$i18n.locale,
          };

          if (this.billingClient) confirmData.client = this.billingClient.id;

          this.$api({
            url: `registration/${data.data.id}/confirm`,
            method: 'put',
            data: confirmData,
          })
            .then((data) => {
              this.$flash(this.$tr('success'), 'success');

              this.$store.commit('eventRegistrationForm/confirmRegistration', [this.eventId, data.data]);

              // Remove autofill data to include newly added people later
              this.$db(`autofillDebaters-event${this.eventId}`, this.DB_DEL);
              this.$db(`autofillTeams-event${this.eventId}`, this.DB_DEL);

              this.$router.push({
                name: 'event-confirmation',
                params: {
                  ...this.$route.params,
                  confirmation: this.$tr('paths.eventParams.confirmation', null, false),
                }
              });
            })
            .catch((data) => {
              if (data.response && data.response.data) {
                const { message } = data.response.data;

                if (message) {
                  return this.$flash(
                    this.$tr(`validation.${message}`, null, false),
                    'error',
                  );
                }
              }

              this.$flash(this.$tr('error'), 'error');
            })
            .finally(() => {
              this.loading = false;
            });
        })
        .catch(([data, person = null]) => {
          this.loading = false;

          if (data.response && data.response.data) {
            const { message: originalMessage } = data.response.data;

            const message = originalMessage ?? (data.response.data.parent_email ? 'parentEmail' : null);

            if (message) {
              return this.$flash(
                this.$tr(`validation.${message}`, {
                  person: `${person.person.name} ${person.person.surname}`,
                }, false),
                'error',
              );
            }
          }

          this.$flash(this.$tr('error'), 'error');
        });
    },

    // Create or update person
    _createPerson(person) {
      return new Promise((resolve, reject) => {
        const { autofill } = person;

        // Person is just autofilled and not edited -> return ID
        if (autofill && !autofill.edited) return resolve(autofill.id);

        // Person already created -> don't recreate
        if (person.registration.person) return resolve(person.registration.person);

        // Person is autofilled and edited / newly created
        this.$api({
          url: `person${autofill ? `/${autofill.id}` : ''}`,
          data: person.person,
          method: autofill ? 'put' : 'post',
          alerts: false,
        })
          .then((data) => {
            resolve(data.data.id);
          })
          .catch((data) => {
            reject(data);
          });
      });
    },

    removePerson(index: number) {
      const personAutofilled = this.formData[index].autofill;

      // Person was autofilled -> mark as unregistered again
      if (personAutofilled) {
        this.$db(
          `autofillDebaters-event${this.eventId}`,
          this.$db(`autofillDebaters-event${this.eventId}`)
            .map((item) => ({
              ...item,
              registered: item.id === personAutofilled.id ? false : item.registered,
            })),
          true,
        );
      }

      this.$store.commit('eventRegistrationForm/removeEventRegistration', [this.eventId, index]);

      if (!this.formData.length) this.$router.push(this.rolePickRoute);
    },

    changeBilling(client) {
      this.billingClient = client;
    },
  },
  components: {
    CountrySelect,
    PersonCard,
    billingMenu,
  },
});
</script>
