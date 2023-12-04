<template>
  <q-page class="flex flex-center content-center">
    <div class="row full-width text-center">
      <!-- Template strings cannot be used in [src] attributes -->
      <img class="q-mx-auto" alt="ADK Pride Logo" src="../assets/pride_logo_text.png"
           v-if="$isPride && $organizer === 'adk'" />
      <img class="q-mx-auto" alt="PDS Logo" src="../assets/logo_pds_text.svg"
           v-else-if="$organizer === 'pds'" />
      <img class="q-mx-auto" alt="EuroSDC Logo" src="../assets/logo_eurosdc_text.svg"
           v-else-if="$organizer === 'eurosdc'" />
      <img class="q-mx-auto" alt="ADK Logo" src="../assets/logo_text.png" v-else />
    </div>
    <div class="row q-mt-xl flex-center home-infocard-wrapper"
         v-if="Object.keys(events).length || !$auth.isLoggedIn()">
      <q-card v-if="Object.keys(events).length">
        <q-list bordered separator padding>
          <q-item
            v-for="event in events"
            v-bind:key="event.id"
            :to="$translatedRouteLink({
              name: 'event',
              params: {
                id: event.id
              },
            })">
            <q-item-section>
              <q-item-label>{{ $tr(event.name) }}</q-item-label>
              <q-item-label caption><EventDate :event="event" /></q-item-label>
            </q-item-section>
          </q-item>

        </q-list>
      </q-card>
      <q-card class="home-infocard" v-if="!$auth.isLoggedIn()">
        <p>
          {{ $tr('general.loginNeeded') }}
          <br>
          {{ $tr('general.noAccountYet') }}
        </p>
        <div class="text-center">
          <q-btn
            class="q-ma-md hidden-link"
            :to="$path('auth.login')"
            icon="fas fa-sign-in-alt"
            :label="$tr('auth.login.link')"
            color="primary"
            size="lg"
          />
          <q-btn
            class="q-ma-md hidden-link"
            :to="$path('auth.signUp')"
            icon="fas fa-user-plus"
            :label="$tr('auth.signUp.link')"
            color="blue-9"
            size="lg"
          />
        </div>
      </q-card>
    </div>
  </q-page>
</template>

<style></style>

<script lang="ts">
import { defineComponent } from 'vue';
import { Event } from 'src/types/event';
import EventDate from 'components/Event/EventDate.vue';

export default defineComponent({
  name: 'Homepage',
  components: { EventDate },
  computed: {
    events(): Event[] {
      // eslint-disable-next-line @typescript-eslint/no-unsafe-member-access
      return <Event[]> this.$store.getters['events/eventsCurrent'];
    },
  },
});
</script>
