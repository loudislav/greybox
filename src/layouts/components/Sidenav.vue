<template>
  <q-drawer
      :model-value="modelValue"
      @update:model-value="$emit('update:model-value', $event)"
      bordered
  >
    <q-list>
      <q-scroll-area>
      <q-item :to="$path('home')" exact>
        <q-item-section avatar>
          <q-icon name="fas fa-home" />
        </q-item-section>
        <q-item-section>
          <q-item-label>{{ $tr('general.homepage') }}</q-item-label>
        </q-item-section>
      </q-item>

      <q-item-label header>{{ $tr('event.link') }}</q-item-label>
      <q-item
          v-for="event in events"
          v-bind:key="event.id"
          :class="
            // Needed for switching locales
            ($route.name === 'event' || $route.meta?.translationName === 'event')
            && parseInt($route.params.id) === event.id
              ? 'q-router-link--active'
              : ''
          "
          :to="$translatedRouteLink({
            name: 'event',
            params: {
              id: event.id
            },
          })">
        <q-item-section avatar>
          <q-icon name="fas fa-trophy" />
        </q-item-section>
        <q-item-section>
          <q-item-label>{{ $tr(event.name) }}</q-item-label>
        </q-item-section>
      </q-item>
      <div v-if="!Object.keys(events).length" class="empty-info">
        {{ $tr('event.empty') }}
      </div>

      <div v-if="Object.keys(eventsClosed).length">
        <q-item-label header>{{ $tr('event.linkClosed') }}</q-item-label>
      </div>
      <q-item
          v-for="event in eventsClosed"
          v-bind:key="event.id"
          :class="
            // Needed for switching locales
            ($route.name === 'event' || $route.meta?.translationName === 'event')
            && parseInt($route.params.id) === event.id
              ? 'q-router-link--active'
              : ''
          "
          :to="$translatedRouteLink({
            name: 'event',
            params: {
              id: event.id
            },
          })">
        <q-item-section avatar>
          <q-icon name="fas fa-trophy" />
        </q-item-section>
        <q-item-section>
          <q-item-label>{{ $tr(event.name) }}</q-item-label>
        </q-item-section>
      </q-item>

      <template v-if="$auth.isAdmin() || ($auth.user()?.organizedEventsIds ?? []).length">
        <q-item-label header>
          {{ $tr($auth.isAdmin() ? 'admin.title' : 'event.types.organizer') }}
        </q-item-label>
        <q-item :to="$path('admin.newEvent')" v-if="$auth.isAdmin()">
          <q-item-section avatar>
            <q-icon name="fas fa-plus" />
          </q-item-section>
          <q-item-section>
            <q-item-label>{{
                $tr('admin.newEvent.link')
              }}
            </q-item-label>
          </q-item-section>
        </q-item>
        <q-item :to="$path('admin.events')">
          <q-item-section avatar>
            <q-icon name="fas fa-trophy" />
          </q-item-section>
          <q-item-section>
            <q-item-label>{{
                $tr('admin.events.link')
              }}
            </q-item-label>
          </q-item-section>
        </q-item>
      </template>

      <q-item-label header>{{ $tr('general.user') }}</q-item-label>
      <template v-if="$auth.isLoggedIn() && $auth.user()">

        <q-item :href="$auth.user().wrappedUrl" clickable v-if="$auth.user().wrappedUrl">
          <q-item-section avatar>
            <q-icon name="fas fa-gift" />
          </q-item-section>

          <q-item-section>{{
              $tr('user.wrapped')
            }}
          </q-item-section>
        </q-item>

        <q-item :to="$path('user.myRegistrations')" clickable>
          <q-item-section avatar>
            <q-icon name="fas fa-list-alt" />
          </q-item-section>

          <q-item-section>{{
              $tr('user.myRegistrations.link')
            }}
          </q-item-section>
        </q-item>

        <q-item :to="$path('myDebates')" clickable v-if="$organizer === 'adk'">
          <q-item-section avatar>
            <q-icon name="fas fa-user-tie" />
          </q-item-section>

          <q-item-section>{{
              $tr('myDebates.link')
            }}
          </q-item-section>
        </q-item>

        <q-item :to="$path('auth.accountSettings')" clickable>
          <q-item-section avatar>
            <q-icon name="fas fa-cog" />
          </q-item-section>

          <q-item-section>{{
              $tr('auth.accountSettings.link')
            }}
          </q-item-section>
        </q-item>

        <!--
                TODO - implement link to server to download personal data
                <q-item clickable>
                  <q-item-section avatar>
                    <q-icon name="fas fa-download" />
                  </q-item-section>

                  <q-item-section
                    >{{ $tr("general.downloadPersonalData") }}
                  </q-item-section>
                </q-item>
                -->

        <q-item :to="$path('auth.logout')" clickable>
          <q-item-section avatar>
            <q-icon name="fas fa-sign-out-alt" />
          </q-item-section>

          <q-item-section>{{ $tr('auth.logout.link') }}</q-item-section>
        </q-item>
      </template>
      <template v-else>
        <q-item clickable :to="$path('auth.login')">
          <q-item-section avatar>
            <q-icon name="fas fa-sign-in-alt" />
          </q-item-section>

          <q-item-section>{{ $tr('auth.login.link') }}</q-item-section>
        </q-item>

        <q-item clickable :to="$path('auth.signUp')">
          <q-item-section avatar>
            <q-icon name="fas fa-user-plus" />
          </q-item-section>

          <q-item-section>{{ $tr('auth.signUp.link') }}</q-item-section>
        </q-item>

        <q-item clickable :to="$path('auth.passwordReset')">
          <q-item-section avatar>
            <q-icon name="fas fa-undo" />
          </q-item-section>

          <q-item-section>{{ $tr('auth.passwordReset.link') }}</q-item-section>
        </q-item>
      </template>

      <q-item-label header>{{ $tr('general.essentialLinks') }}</q-item-label>
      <q-item
          clickable
          tag="a"
          rel="noopener"
          target="_blank"
          href="https://debatovani.cz/greybox/"
          v-if="$organizer === 'adk'"
      >
        <q-item-section avatar>
          <q-icon name="fas fa-chart-bar" />
        </q-item-section>
        <q-item-section>
          <q-item-label>{{ $tr('general.statistics') }}</q-item-label>
          <q-item-label caption>greybox v1.0</q-item-label>
        </q-item-section>
      </q-item>
      <q-item :to="$path('about')">
        <q-item-section avatar>
          <q-icon name="fas fa-info-circle" />
        </q-item-section>
        <q-item-section>
          <q-item-label>{{ $tr('general.aboutUs') }}</q-item-label>
        </q-item-section>
        </q-item>
      </q-scroll-area>
    </q-list>
  </q-drawer>
</template>

<script lang="ts">
import { defineComponent } from 'vue';
import { Event } from 'src/types/event';

export default defineComponent({
  name: 'Sidenav',
  props: {
    modelValue: Boolean,
  },
  computed: {
    events(): Event[] {
      // eslint-disable-next-line @typescript-eslint/no-unsafe-member-access
      return <Event[]> this.$store.getters['events/eventsBeforeRegistration'];
    },
    eventsClosed(): Event[] {
      // eslint-disable-next-line @typescript-eslint/no-unsafe-member-access
      return <Event[]> this.$store.getters['events/eventsClosed'];
    },
  },
  async created() {
    await this.$store.dispatch('events/load');
  },
  emits: [
    'update:model-value',
  ],
});
</script>
