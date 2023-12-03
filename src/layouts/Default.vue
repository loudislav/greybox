<template>
  <q-layout
    view="lHh Lpr lFf"
    :class="'bg-grey-2 page-' + $route.name"
  >
    <q-header elevated>
      <q-toolbar>
        <q-btn
          flat
          dense
          round
          @click="toggleDrawerMenu"
          aria-label="Menu"
          icon="fas fa-bars"
          class="lt-md"
          ref="toggleDrawerMenuButton"
        />

        <q-toolbar-title>
          <span>
            <q-avatar size="35px">
              <!-- Template strings cannot be used in [src] attributes -->
              <img src="../assets/pride_logo.png" alt="logo"
                   v-if="$isPride && $organizer === 'adk'" />
              <img src="../assets/logo_pds.svg" alt="logo" v-else-if="$organizer === 'pds'" />
              <img src="../assets/logo.svg" alt="logo" v-else />
            </q-avatar>

            <template v-if="$organizer === 'pds'">
              Prague Debate Spring
            </template>
            <template v-else-if="$organizer === 'eurosdc'">
              EuroSDC
            </template>
            <template v-else>
              greybox 2.0
              <span v-if="env.MODE !== 'production'" class="mode-flag">
                <template v-if="env.STAGE === 'debug'">
                  debug
                </template>
                <template v-else-if="env.STAGE === 'local'">
                  dev
                </template>
              </span>
            </template>
          </span>
        </q-toolbar-title>

        <q-avatar
          size="25px"
          class="lang-switch"
          v-for="(lang, locale) in languages"
          v-bind:key="locale"
        >
          <img
            :src="require(`../assets/${locale}_flag.png`)"
            :alt="lang.native"
            :title="lang.native"
            :class="{ 'flag-dimmed': $i18n.locale !== locale }"
            @click="switchLocale(locale)"
          />
        </q-avatar>

        <q-btn-dropdown
          stretch
          flat
          content-class="no-maxwidth-menu"
          v-if="$auth.isLoggedIn() && $auth.user()"
        >
          <template v-slot:label>
            <q-avatar
              :style="
                'background-color: ' + $stringToHslColor($auth.user().username)
              "
            >
              <img
                src="https://cdn.quasar.dev/img/avatar.png"
                v-if="!true"
                alt="Avatar"
              />
              <template v-else>{{
                  $auth
                    .user()
                    .username
                    .substr(0, 1)
                    .toUpperCase()
                }}
              </template>
            </q-avatar>
            <span class="username">
              {{ $auth.user().username }}
            </span>
          </template>
          <q-list>
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

                <q-item-section>{{
                  $tr("general.downloadPersonalData")
                }}</q-item-section>
              </q-item>
              -->

            <q-item :to="$path('auth.logout')" clickable>
              <q-item-section avatar>
                <q-icon name="fas fa-sign-out-alt" />
              </q-item-section>

              <q-item-section>{{ $tr('auth.logout.link') }}</q-item-section>
            </q-item>
          </q-list>
        </q-btn-dropdown>
        <q-btn stretch flat v-else :to="$path('auth.login')" title="Login">
          <q-icon name="fas fa-user" />
        </q-btn>
      </q-toolbar>
    </q-header>

    <sidenav
      v-model="leftDrawerOpen"
      :key="
        $auth.isLoggedIn() && $auth.user() ? $auth.user().id + '-sidenav' : 'sidenav'
      "
    ></sidenav>

    <q-page-container>
      <transition name="fade">
        <div class="full-page-loader" v-if="fullLoader">
          <img alt="Logo ADK" src="../assets/logo.gif" />
        </div>
      </transition>
      <router-view :key="$route.path" />
    </q-page-container>
  </q-layout>
</template>

<script>
import { defineComponent } from 'vue';
import Sidenav from './components/Sidenav';
import i18nConfig from '../translation/config';
import { switchLocale, switchQuasarLanguage } from '../boot/i18n';

export default defineComponent({
  name: 'LayoutDefault',

  components: {
    // eslint-disable-next-line @typescript-eslint/no-unsafe-assignment
    Sidenav,
  },

  data() {
    return {
      // eslint-disable-next-line @typescript-eslint/no-unsafe-assignment
      leftDrawerOpen:
        this.$q.platform.is.desktop
        && localStorage.getItem('leftDrawerOpen') !== 'false',
      user: null,
      fullLoader: 0, // number of active loadings
      languages: i18nConfig.languages,
    };
  },
  methods: {
    switchLocale,
    toggleDrawerMenu() {
      this.leftDrawerOpen = !this.leftDrawerOpen;
      localStorage.setItem('leftDrawerOpen', this.leftDrawerOpen);
    },
    checkDrawerOpened() {
      // eslint-disable-next-line @typescript-eslint/no-unsafe-member-access
      const toggleButtonVisible = this.$refs.toggleDrawerMenuButton.$el.offsetParent !== null;

      // Desktop
      if (!toggleButtonVisible) {
        this.leftDrawerOpen = true;
        localStorage.setItem('leftDrawerOpen', true);
      }
    },
  },

  created() {
    // localization
    setTimeout(() => {
      if (this.$auth.user() && this.$auth.user().preferred_locale) {
        void switchQuasarLanguage(this.$auth.user().preferred_locale);
        if (this.$auth.user().preferred_locale !== this.$i18n.locale) {
          void switchLocale(this.$auth.user().preferred_locale);
        }
      }
    }, 500);

    this.$bus.$on('fullLoader', (value) => {
      if (value) {
        this.fullLoader += 1;
      } else {
        this.fullLoader -= 1;
      }

      if (this.fullLoader < 0) this.fullLoader = 0;
    });
  },

  mounted() {
    this.checkDrawerOpened();
    window.addEventListener('resize', () => this.checkDrawerOpened(), true);
  },

  unmounted() {
    window.removeEventListener('resize', () => this.checkDrawerOpened(), true);
  },
});
</script>
