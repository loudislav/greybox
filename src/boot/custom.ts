/* eslint-disable */
import { Locale, TranslateResult, VueMessageType } from 'vue-i18n';
import { LocaleMessageValue } from '@intlify/core-base';

const Bugsnag = require('@bugsnag/js');
const BugsnagPluginVue = require('@bugsnag/plugin-vue');
import config from '../config';
import { Notify } from 'quasar';

const smartformModule = require('@smartform.cz/smartform');
import { boot } from 'quasar/wrappers';
import { i18n, TranslatedString } from 'boot/i18n';
import i18nConfig from '../translation/config';
import store, { State } from '../store';
import { Store } from 'vuex';
import assert from 'assert';
import { LocationAsRelativeRaw, MatcherLocationAsPath } from 'vue-router';
import { translatedRouteLink } from 'src/router/helpers';
import { EventOrganizer } from 'src/types/event';

export type TranslationValue = TranslateResult | LocaleMessageValue<VueMessageType> | {};

export type InfiniteObject = {
  [key: string]: string | number | undefined | null | InfiniteObject
};

export type DBValue = undefined | string | number | boolean | null | InfiniteObject | DBValue[];

// Required for TypeScript to work with global properties
declare module '@vue/runtime-core' {
  interface ComponentCustomProperties {
    $db: (key: any, value?: DBValue, personal?: boolean) => DBValue;
    $flash: (message: string | TranslationValue, type?: string, icon?: string | undefined, timeout?: number) => Function;
    $organizer: EventOrganizer;
    $path: (route: string) => string;
    $tr: (key: string | TranslatedString, options?: Record<string, unknown> | null, usePrefix?: boolean, lang?: Locale | null) => TranslationValue;
    $store: Store<State>;
    $translatedRouteLink: (route: string | LocationAsRelativeRaw) => MatcherLocationAsPath;
  }
}

export const $tr = function (
  key: string | TranslatedString, options: Record<string, unknown> | null = null, usePrefix: boolean = true, lang: Locale | null = null,
): TranslationValue {
  // Translate object received from APIimport { LocaleMessageValue } from '@intlify/core-base';
  const {
    locale,
    t,
    tm,
  } = i18n.global;
  if (typeof key === 'object') {
    let activeLocale = <keyof TranslatedString>(lang ?? (locale || i18nConfig.default));

    if (!key || !key[activeLocale]) return '';

    return key[activeLocale];
  }

  // Translate string from JSON
  // @ts-ignore
  let prefix = this ? this.$.data.translationPrefix : null;

  // Use prefix
  if (prefix && usePrefix) key = prefix + key;

  if (options !== null) {
    return t(key, options);
  }
  if (lang !== null) {
    return t(key, lang);
  }
  return tm(key);
};

export const $path = function (route: string): string {
  return '/' + $tr(`paths.${route}`, null, false);
};

export const $flash = function (message: string | TranslationValue, type: string = 'info', icon: string | undefined = undefined, timeout: number = 3500): Function | undefined {
  if (typeof message !== 'string') {
    return;
  }

  let color: string | undefined = undefined;
  if (type === 'success' || type === 'done') {
    color = 'green';
    if (!icon) icon = 'check';
  } else if (type === 'error' || type === 'fail') {
    color = 'red';
    if (!icon) icon = 'times';
  }

  return Notify.create({
    color,
    icon: icon ? 'fas fa-' + icon : undefined,
    message: message,
    html: true,
    position: 'top-right',
    timeout,
    closeBtn: '-',
  });
};

export const $slug = (original: string): string => {
  const a =
    'àáäâãåăæąçćčđďèéěėëêęğǵḧìíïîįłḿǹńňñòóöôœøṕŕřßşśšșťțùúüûǘůűūųẃẍÿýźžż·/_,:;';
  const b =
    'aaaaaaaaacccddeeeeeeegghiiiiilmnnnnooooooprrsssssttuuuuuuuuuwxyyzzz------';
  const p = new RegExp(a.split('')
    .join('|'), 'g');

  return original
    .toString()
    .toLowerCase()
    .replace(/\s+/g, '-') // Replace spaces with -
    .replace(p, (c: string) => b.charAt(a.indexOf(c))) // Replace special characters
    .replace(/&/g, '-and-') // Replace & with 'and'
    .replace(/[^\w-]+/g, '') // Remove all non-word characters
    .replace(/--+/g, '-') // Replace multiple - with single -
    .replace(/^-+/, '') // Trim - from start of text
    .replace(/-+$/, ''); // Trim - from end of text
};

export const $slugTranslation = (original: TranslatedString): TranslatedString => ({
  ...(original as object),
  cs: $slug(original.cs),
  en: $slug(original.en),
});

export const $organizer: EventOrganizer = <EventOrganizer> process.env.ORGANIZER;

export const $isPride = (new Date()).getMonth() == 5;

// A bit dirty
if ($organizer == 'pds')
  document.title = 'Prague Debate Spring';
else if ($organizer == 'eurosdc')
  document.title = 'EuroSDC';

// Convert array of objects into object of objects (with IDs as keys)
export const $makeIdObject = (array: any) => {
  let result: Record<string | number, any> = {};

  array.map((item: Record<string, string | number>) => {
    result[item.id] = item;
  });

  return result;
};

export const $setTitle = (title?: string | undefined) => {
  if (title) {
    document.title = title + ' | Greybox 2.0';
  } else {
    document.title = 'Greybox 2.0';
  }
}

export default boot(({ app }) => {
  app.use(store);

  // $isPDS bool
  app.config.globalProperties.$organizer = $organizer;
  app.config.globalProperties.$isPride = $isPride;

  // Initialize SmartForms
  smartformModule.load();
  // @ts-ignore
  window.smartform.beforeInit = () => {
    // @ts-ignore
    window.smartform.setClientId('8ndPcVUJ5B');
  };

  // Bugsnag error monitoring
  if (typeof process.env.BUGSNAG_KEY === 'string') {
    Bugsnag.start({
      apiKey: process.env.BUGSNAG_KEY,
      plugins: [new BugsnagPluginVue()],
      releaseStage: process.env.STAGE,
    });

    const bugsnagVue = Bugsnag.getPlugin('vue');
    app.use(bugsnagVue);
  }

  // Mixins
  const DB_DELETION_CONSTANT = 'DELETE-THIS-DATABASE-ITEM'; // when DB item is set to this value, it will be deleted
  let uuid = 0;
  app.mixin({
    data() {
      return {
        apiSettings: config.api,
        env: process.env.FULL_ENV,
        DB_DEL: DB_DELETION_CONSTANT,
      };
    },
    // Generate unique ID for every component
    beforeCreate: function () {
      this.uuid = uuid.toString();
      uuid += 1;
    },
    methods: {
      // Translation simplification
      // If translationPrefix data is set in component, it will be automcatically placed in front of translation keys
      $tr,

      // Check if translation exists
      $trExists: function (key: string, usePrefix: boolean = true) {
        // Translate string from JSON
        let prefix = this ? this.$.data.translationPrefix : null;

        // Use prefix
        if (prefix && usePrefix) key = prefix + key;

        return this.$te(key);
      },

      // Get path to route
      $path,

      $translatedRouteLink: translatedRouteLink,

      $stringToHslColor: function (str: string, s: number = 100) {
        let hash = 0;
        for (let i = 0; i < str.length; i++) {
          hash = str.charCodeAt(i) + ((hash << 5) - hash);
        }

        const h = hash % 360;
        const l = Math.abs((h * 100) % 40) + 20;

        return 'hsl(' + h + ', ' + s + '%, ' + l + '%)';
      },

      // Flash
      $flash,

      // Create slug from string
      // Source: https://codepen.io/tatthien/pen/xVBxZQ
      $slug,

      $slugTranslation,

      $makeIdObject,

      // Simple global database interface

      // @key: Index for the value to be stored under
      // @value: null = read value; undefined = delete value; other = set value
      // @personal: true = value will be uncached on logout
      $db(key: string, value: DBValue = null, personal: boolean = false): DBValue {
        let dbKey = personal ? 'dbPersonal' : 'db';

        // Workaround for personal GET and DELETE
        if (
          (value === null || value === DB_DELETION_CONSTANT) &&
          typeof app.config.globalProperties[dbKey][key] == 'undefined'
        ) {
          dbKey = 'dbPersonal';
        }

        // Select request
        if (value === null) return app.config.globalProperties[dbKey][key];

        // Delete request
        if (value === DB_DELETION_CONSTANT) {
          delete app.config.globalProperties[dbKey][key];
          return;
        }

        // Insert/update request
        return app.config.globalProperties[dbKey][key] = value;
      },

      $dbFlushPersonal(): void {
        app.config.globalProperties['dbPersonal'] = {};
      },

      // Show basic confirm dialog
      $confirm(settings: any) {
        let defaults = {
          class: 'simple-confirm-dialog',
          title: this.$tr('general.confirmModal.title', null, false),
          message: null,
          cancel: this.$tr('general.confirmModal.cancel', null, false),
          ok: {
            label: this.$tr('general.confirmModal.confirm', null, false),
          },
        };

        // Merge settings with defaults
        settings = { ...defaults, ...settings };

        if (settings.confirm) settings.ok.label = settings.confirm;
        return this.$q.dialog(settings);
      },
    },
  });

  // Custom cache DB mechanism
  app.config.globalProperties.db = {};
  app.config.globalProperties.dbPersonal = {};

});

export const customCompareStrings = (a: string, b: string) => {
  if (a === null || a === undefined) {
    return 1;
  } if (b === null || b === undefined) {
    return -1;
  }
  return a.localeCompare(b, 'cs');
};
