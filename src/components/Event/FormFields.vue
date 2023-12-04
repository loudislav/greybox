<template>
  <q-form @submit="sendForm" @reset="resetForm" ref="q-form">
    <div class="row q-col-gutter-md q-pb-sm">
      <q-input
        outlined
        v-model="values.name"
        :label="$tr('fields.name') + ' *'"
        class="col-12 col-sm-6"
        lazy-rules
        :rules="[$validators.nonEmpty]"
      />

      <q-input
        outlined
        v-model="values.surname"
        :label="$tr('fields.surname') + ' *'"
        class="col-12 col-sm-6"
        lazy-rules
        :rules="[$validators.nonEmpty]"
      />
    </div>

    <q-input
      v-if="requireEmail"
      v-model="values.email"
      class="q-mt-xs"
      type="email"
      outlined
      :label="$tr('auth.fields.email', null, false) + ' *'"
      lazy-rules
      :rules="[$validators.nonEmpty, $validators.email]"
    >
      <template v-slot:prepend>
        <q-icon name="fas fa-at" />
      </template>
    </q-input>

    <q-select
      outlined
      v-if="requireSchoolYear"
      v-model="values.schoolYear"
      :options="schoolYears"
      :label="`${$tr('fields.schoolYear')} *`"
      class="q-pt-sm q-mb-sm col-12 col-md-4"
      lazy-rules
      use-input
      input-debounce="0"
      :rules="[val => val || $tr('general.form.errors.nonEmpty', null, false)]"
    >
      <template v-slot:prepend>
        <q-icon name="fas fa-graduation-cap" />
      </template>
    </q-select>

    <q-select
            outlined
            v-model="values.gender"

            :options="genderOptions"
            option-value="label"
            :label="$tr('fields.gender') + ' *'"
            class="q-pt-sm q-mb-sm col-12"
            lazy-rules
            v-if="requireGender"
            :rules="[val =>
          val ||
          $tr('general.form.errors.nonEmpty', null, false)
        ]"
    >
        <template v-slot:prepend>
            <q-icon name="fas fa-venus-mars" />
        </template>
        <template v-slot:append>
            <q-icon name="fas fa-info-circle" />
            <q-tooltip
                    anchor="top middle"
                    self="bottom middle"
                    :offset="[0, -10]"
            >
                {{ $tr('fieldNotes.gender') }}
            </q-tooltip>
        </template>
    </q-select>

    <div
      class="block"
      v-if="accommodationType !== 'required' && accommodationType !== 'none'"
    >
      <q-checkbox
        v-model="values.accommodation"
        v-if="accommodationType === 'opt-in'"
        :true-value="true"
        :false-value="false"
        :label="$tr('fields.accommodationOptIn')"
      />
      <q-checkbox
        v-model="values.accommodation"
        v-else
        :true-value="false"
        :false-value="true"
        :label="$tr('fields.accommodation')"
      />
      <q-icon name="fas fa-info-circle" class="q-pl-sm">
        <q-tooltip anchor="top middle" self="bottom middle" :offset="[0, 0]">
          {{ $tr('fieldNotes.accommodation') }}
        </q-tooltip>
      </q-icon>
    </div>

    <!-- details needed for accommodation -->
    <div
      class="q-field--with-bottom"
      :class="{ 'form-conditional-block': accommodationType !== 'required' }"
      v-show="values.accommodation === true"
      v-if="accommodationType !== 'none'"
    >
      <div class="row q-col-gutter-sm">
        <div class="col-12 q-field" style="color: rgba(0,0,0,0.54);">
          {{ $tr('fields.birthdate') }} *
        </div>
        <q-select
          outlined
          v-model="values.birthDay"
          :options="days"
          option-value="label"
          :label="$tr('fields.birthDay')"
          class="q-pt-sm q-mb-sm col-12 col-md-4"
          data-select-value="birthDay"
          data-select-options="days"
          lazy-rules
          use-input
          @filter="filterDaySelect"
          input-debounce="0"
          :rules="[val =>
            values.accommodation === false ||
            val ||
            $tr('general.form.errors.nonEmpty', null, false)
          ]"
        >
          <template v-slot:prepend>
            <q-icon name="fas fa-calendar-alt" />
          </template>
        </q-select>

        <q-select
          outlined
          v-model="values.birthMonth"
          :options="months"
          option-value="label"
          :option-label="item => $tr(item.label, null, false)"
          :label="$tr('fields.birthMonth')"
          class="q-pt-sm q-mb-sm col-12 col-md-4"
          data-select-value="birthMonth"
          data-select-options="months"
          lazy-rules
          use-input
          @filter="filterMonthSelect"
          input-debounce="0"
          :rules="[val =>
            values.accommodation === false ||
            val ||
            $tr('general.form.errors.nonEmpty', null, false)
          ]"
        >
          <template v-slot:prepend>
            <q-icon name="fas fa-calendar-alt" />
          </template>
        </q-select>

        <q-select
          outlined
          v-model="values.birthYear"
          :options="years"
          :label="$tr('fields.birthYear')"
          class="q-pt-sm q-mb-sm col-12 col-md-4"
          data-select-value="birthYear"
          data-select-options="years"
          lazy-rules
          use-input
          @filter="filterYearSelect"
          input-debounce="0"
          :rules="[val =>
            values.accommodation === false ||
            val ||
            $tr('general.form.errors.nonEmpty', null, false)
          ]"
        >
          <template v-slot:prepend>
            <q-icon name="fas fa-calendar-alt" />
          </template>
        </q-select>
      </div>

      <mask-input
        outlined
        v-model="values.id_number"
        :label="$tr('fields.id_number')"
        class="q-pt-sm"
        mask="#########"
        fill-mask="_"
        :hint="$tr('fieldNotes.example') + ' 123456789'"
        lazy-rules
        :rules="[
          val =>
            values.accommodation === false ||
            val === '_________' ||
            val === '#########' ||
            !val ||
            val.toString().match(/\d{9}/) ||
            $tr('general.form.errors.nonEmpty', null, false)
        ]"
      >
        <template v-slot:prepend>
          <q-icon name="fas fa-id-card" />
        </template>
        <template v-slot:append>
          <q-icon name="fas fa-info-circle" />
          <q-tooltip
            anchor="top middle"
            self="bottom middle"
            :offset="[0, -10]"
          >
            {{ $tr('fieldNotes.id_number') }}
          </q-tooltip>
        </template>
      </mask-input>

      <q-input
        outlined
        v-model="values.street"
        :label="$tr('fields.street') + ' *'"
        class="q-pt-sm"
        :input-class="
          'smartform-street-and-number ' + 'smartform-instance-' + uuid
        "
        lazy-rules
        :rules="[
          val =>
            values.accommodation === false ||
            (val && val.length > 0) ||
            $tr('general.form.errors.nonEmpty', null, false)
        ]"
      >
        <template v-slot:prepend>
          <q-icon name="fas fa-home" />
        </template>
      </q-input>

      <q-input
        outlined
        v-model="values.city"
        :label="$tr('fields.city') + ' *'"
        class="q-pt-sm"
        :input-class="'smartform-city ' + 'smartform-instance-' + uuid"
        lazy-rules
        :rules="[
          val =>
            values.accommodation === false ||
            (val && val.length > 0) ||
            $tr('general.form.errors.nonEmpty', null, false)
        ]"
      >
        <template v-slot:prepend>
          <q-icon name="fas fa-city" />
        </template>
      </q-input>

      <template v-if="$organizer === 'pds'">
        <mask-input
            outlined
            v-model="values.zip"
            :label="$tr('fields.zip') + ' *'"
            class="q-pt-sm"
            :input-class="'smartform-zip ' + 'smartform-instance-' + uuid"
            mask="### ##"
            fill-mask="_"
            :hint="$tr('fieldNotes.example') + ' 796 01'"
        >
          <template v-slot:prepend>
            <q-icon name="fas fa-file-archive" />
          </template>
        </mask-input>
      </template>
      <template v-else>
        <mask-input
            outlined
            v-model="values.zip"
            :label="$tr('fields.zip') + ' *'"
            class="q-pt-sm"
            :input-class="'smartform-zip ' + 'smartform-instance-' + uuid"
            mask="### ##"
            fill-mask="_"
            :hint="$tr('fieldNotes.example') + ' 796 01'"
            lazy-rules
            :rules="[
          val =>
            values.accommodation === false ||
            (val && val.toString().match(/\d{3} ?\d{2}/)) ||
            $tr('general.form.errors.nonEmpty', null, false)
        ]"
        >
          <template v-slot:prepend>
            <q-icon name="fas fa-file-archive" />
          </template>
        </mask-input>
      </template>

    </div>

    <!-- -SCHOOL FIELD-
    <q-input
      outlined
      v-model="values.school"
      :label="$tr('fields.school') + ' *'"
      class="q-mt-sm"
      lazy-rules
      :rules="[$validators.nonEmptyField]"
    >
      <template v-slot:prepend>
        <q-icon name="fas fa-school" />
      </template>
    </q-input>
    -->
    <!--
        <q-input
          outlined
          v-model="values.phone"
          :label="$tr('fields.phone')"
          mask="+### #########"
          fill-mask="#"
          class="q-pt-sm"
          hint="Např.: +420 123456789"
        >
          <template v-slot:prepend>
            <q-icon name="fas fa-phone-alt" />
          </template>
        </q-input>
        -->

    <div
      class="block q-mt-sm checkbox-wrapper"
      v-if="mealType !== 'required' && mealType !== 'none'"
    >
      <q-checkbox
        v-model="values.meals"
        v-if="mealType === 'opt-in'"
        :true-value="true"
        :false-value="false"
        :label="$tr('fields.mealsOptIn')"
      />
      <q-checkbox
        v-model="values.meals"
        v-else
        :true-value="false"
        :false-value="true"
        :label="$tr('fields.meals')"
      />
      <!--
      <q-icon name="fas fa-info-circle" class="q-pl-sm">
        <q-tooltip anchor="top middle" self="bottom middle" :offset="[0, 0]">
          {{ $tr("fieldNotes.meals") }}
        </q-tooltip>
      </q-icon>
      -->
    </div>

    <div
      class="block"
      :class="{ 'form-conditional-block': mealType !== 'required' }"
      v-if="
        mealType !== 'none' &&
          values.meals === true &&
          Object.keys(possibleDietsOptions).length !== 0
      "
    >
      <q-select
        outlined
        v-model="values.dietary_requirement"
        :options="possibleDietsOptions"
        option-value="label"
        :label="$tr('fields.diet')"
        class="q-pt-sm q-mb-sm col-12"
        data-select-value="diet"
        data-select-options="possibleDiets"
        lazy-rules
        :rules="[val => val || $tr('general.form.errors.nonEmpty', null, false)]"
      >
        <template v-slot:prepend>
          <q-icon name="fas fa-utensils" />
        </template>
      </q-select>
    </div>

    <div
      class="block q-mt-sm checkbox-wrapper q-pb-sm"
      v-if="novices"
    >
      <q-checkbox
        v-model="values.novice"
        :true-value="true"
        :false-value="false"
        :label="$tr('fields.novice')"
      />
      <q-icon name="fas fa-info-circle" class="q-pl-sm">
        <q-tooltip anchor="top middle" self="bottom middle" :offset="[0, 0]">
          {{ $tr("fieldNotes.novice") }}
        </q-tooltip>
      </q-icon>
    </div>

    <!-- International event (PDS) option: -->
    <template v-if="requireSpeakerStatus">
      <q-select
        outlined
        v-model="values.speaker_status"
        :options="speakerOptions"
        option-value="label"
        :label="$tr('fields.speakerStatus') + ' *'"
        class="q-pt-sm q-mb-md col-12 col-md-4"
        lazy-rules
        :rules="[val => val || $tr('general.form.errors.nonEmpty', null, false)]"
      >
        <template v-slot:hint>
          <a class="pointer-cursor" @click="showSpeakerStatusModal = true">
            {{ $tr('speakerStatusModal.title') }}
          </a>
        </template>
      </q-select>
      <q-dialog v-model="showSpeakerStatusModal">
        <q-card class="dialog-medium">
          <q-card-section class="row items-center">
            <div class="text-h6">
              {{ $tr('speakerStatusModal.title') }}
            </div>
            <q-space />
            <q-btn icon="fas fa-times" flat round dense v-close-popup />
          </q-card-section>

          <q-card-section>
            <div>
              <span
                v-html="
                  $tr('speakerStatusModal.classification', {
                    status: 'EFL'
                  }) + ':'
                "
              ></span>

              <ul>
                <li>{{ $tr('speakerStatusModal.common') }},</li>
                <li>{{ $tr('speakerStatusModal.efl') }}.</li>
              </ul>
            </div>
            <div>
              <span
                v-html="
                  $tr('speakerStatusModal.classification', {
                    status: 'ESL'
                  })
                "
              ></span>
              <ul>
                <li>{{ $tr('speakerStatusModal.common') }},</li>
                <li>{{ $tr('speakerStatusModal.esl') }}.</li>
              </ul>
            </div>
          </q-card-section>
        </q-card>
      </q-dialog>
    </template>

    <!--
        <template>
          <div class="q-mr-lg">
            <q-separator class="q-pl-md q-mt-sm q-pb-none" />
          </div>
          <h2 class="text-h6 q-pa-xs q-mt-sm">{{ $tr('fields.legalGuardian') }}</h2>
          <q-input
            outlined
            v-model="values.parentName"
            :label="$tr('fields.parentName') + ' *'"
            class="col-12 col-sm-6"
            lazy-rules
            :rules="[$validators.nonEmptyField]"
          >
            <template v-slot:prepend>
            </template>
          </q-input>

          <q-input
            outlined
            v-model="values.parentPhone"
            :label="$tr('fields.parentPhone') + ' *'"
            mask="+### #########"
            fill-mask="#"
            class="q-pt-sm"
            hint="Např.: +420 123456789"
            lazy-rules
            :rules="[$validators.nonEmptyField]"
          >
            <template v-slot:prepend>
              <q-icon name="fas fa-phone-alt" />
            </template>
          </q-input>
          <q-input
            outlined
            type="email"
            v-model="values.parentEmail"
            :label="$tr('fields.parentEmail') + ' *'"
            class="q-pt-sm"
            lazy-rules
            :rules="[$validators.nonEmptyField]"
          >
            <template v-slot:prepend>
              <q-icon name="fas fa-at" />
            </template>
          </q-input>
          <div class="q-mr-lg">
            <q-separator class="q-pl-md q-mt-none q-mb-md q-pb-none" />
          </div>
        </template>
-->
    <q-input
      v-model="values.note"
      class="q-mt-sm"
      outlined
      autogrow
      :label="
        $tr(requireJudingExperience ? 'fields.judgingExp' : 'fields.note')
      "
    >
      <template v-slot:prepend>
        <q-icon name="fas fa-sticky-note" />
      </template>
    </q-input>

    <template v-if="!isTeam">
      <g-d-p-r-checkbox v-model="values.accept" :error="acceptError" />

      <div class="text-center">
        <q-btn
          :label="$tr('buttons.continue')"
          class="q-my-sm"
          type="submit"
          color="primary"
        />
        <q-btn
          :label="$tr('buttons.clear')"
          type="reset"
          color="negative"
          flat
          class="q-ml-sm q-my-sm"
        />
      </div>
    </template>
  </q-form>
</template>

<script>
/* eslint-disable */
import GDPRCheckbox from './GDPRCheckbox';
import MaskInput from './MaskInput';
import config from '../../config';

export default {
  name: 'FormFields',
  components: {
    GDPRCheckbox,
    MaskInput
  },
  props: {
    autofill: Object,
    isTeam: Boolean,
    accommodationType: String,
    mealType: String,
    novices: Boolean,
    possibleDiets: Array,
    role: Number,
    requireEmail: Boolean,
    requireGender: Boolean,
  },

  data() {
    return {
      translationPrefix: 'event.',
      config,
      values: {
        name: null,
        surname: null,
        id_number: null,
        street: null,
        city: null,
        zip: null,
        gender: null,
        // phone: "+420",
        dietary_requirement: null,
        meals: this.mealType !== 'opt-in' && this.mealType !== 'none',
        novice: false,
        accommodation:
          this.accommodationType !== 'opt-in'
          && this.accommodationType !== 'none',
        // parentName: null,
        // parentPhone: "+420",
        // parentEmail: null,
        note: null,
        email: null,
        accept: false,
        schoolYear: null,
        birthDay: null,
        birthMonth: null,
        birthYear: null,
        speaker_status: null,
        // -SCHOOL FIELD- school: null
      },
      acceptError: false,
      selectSearch: null,
      days: [],
      daysAll: [],
      months: [],
      monthsAll: [],
      years: [],
      yearsAll: [],
      schoolYears: [],
      possibleDietsOptions: [],
      showSpeakerStatusModal: false,
      requireSchoolYear: this.$organizer === 'adk' && this.role === config.debaterRoleId, // only for non-PDS debaters
      requireSpeakerStatus: this.$organizer === 'pds' && this.role === config.debaterRoleId, // only for PDS debaters
      requireJudingExperience: this.$organizer === 'pds' && this.role === config.judgeRoleId, // show "Judging experience" instead of note (only for PDS judges)
      speakerOptions: [
        {
          label: this.$tr('event.fields.EFL'),
          value: 'efl',
        },
        {
          label: this.$tr('event.fields.ESL'),
          value: 'esl',
        },
        {
          label: this.$tr('event.fields.ENL'),
          value: 'enl',
        },
      ],
      genderOptions: [
        {
          label: this.$tr('event.fields.genderValues.m'),
          value: 'm',
        },
        {
          label: this.$tr('event.fields.genderValues.f'),
          value: 'f',
        },
        {
          label: this.$tr('event.fields.genderValues.n'),
          value: 'n',
        },
        /*{
          label: this.$tr('event.fields.genderValues.u'),
          value: 'u',
        },*/
      ],
    };
  },

  created() {

    // Load date select options
    for (let i = 1; i <= 31; i++) {
      this.daysAll.push({
        label: `${i}.`,
        value: (`0${i}`).substr(-2),
      });
    }
    for (let i = new Date().getFullYear() - 10; i >= 1900; i--) {
      this.yearsAll.push(i);
    }
    for (let i = 0; i < 12; i++) {
      this.monthsAll[i] = {
        label: `general.months.${i}`,
        value: (`0${i + 1}`).substr(-2),
      };
    }

    const schoolYearsObject = this.$tr('fields.schoolYears');
    this.schoolYears = Object.keys(schoolYearsObject).map((year) => ({
      value: parseInt(year),
      label: schoolYearsObject[year]
    }));

    for (const i in this.possibleDiets) {
      this.possibleDietsOptions[i] = {
        label:
          this.$tr(this.possibleDiets[i].name)
            .charAt(0)
            .toUpperCase() + this.$tr(this.possibleDiets[i].name)
            .slice(1),
        value: this.possibleDiets[i].id,
      };
      // if last:
      if (i == this.possibleDiets.length - 1) {
        this.values.dietary_requirement = {
          label: this.possibleDietsOptions[0].label,
          value: this.possibleDietsOptions[0].value,
        };
      }
    }

    // Smartform autocomplete select
    this.$bus.$on('smartform', (data) => {
      // If instance ID is this form
      if (data.instance.substr(-(this.uuid.length)) == this.uuid) this.values[data.field] = data.value;
    });
  },

  mounted() {
    this._initSmartform();
  },

  emits: [
    'submit',
    'goToRolePick',
    'update:model-value',
  ],

  methods: {
    _initSmartform() {
      // Disable Smart Form Autocomplete for international tournaments
      if (this.$organizer !== 'adk')
        return;

      // Renitialize smartform
      window.smartform.rebindAllForms(true, () => {
        // Loop through instances
        window.smartform.getInstanceIds()
          .forEach((id) => {
            const instance = window.smartform.getInstance(id);

            // Set limit to 3 results for every field
            [
              'smartform-street-and-number',
              'smartform-city',
              'smartform-zip',
            ].forEach((input) => {
              instance.getBox(input)
                .setLimit(3);
            });

            // Run this callback on selection
            instance.setSelectionCallback((element, value, fieldType) => {
              const field = fieldType.substr('10');

              const varName = field !== 'street-and-number' ? field : 'street';

              // Emit global event so other form instances can receive it
              this.$bus.$emit('smartform', {
                instance: id,
                field: varName,
                value,
              });
            });
          });
      });
    },

    sendForm() {
      if (!this.isTeam && !this.values.accept) return !(this.acceptError = true);

      const formData = this.submitData;

      // Check if autofill data have changed or not
      let autofillData = false;
      if (this.autofill) {
        autofillData = {
          id: this.autofill.id,
          edited: false,
        };

        for (const index in formData) {
          if (
            formData[index] != this.autofill[index]
            && index !== 'accommodation'
            && index !== 'meals'
          ) {
            autofillData.edited = true;
          }
        }
      }

      this.$emit('submit', formData, autofillData);
      return {
        formData,
        autofillData,
      };
    },

    resetForm() {
      this.$emit('goToRolePick');
    },

    birthdateFormatter(year, month, day) {
      if (!year && !month && !day) return null;
      return (
        `${year || '0000'
        }-${
          month ? month.value : '00'
        }-${
          day ? day.value : '00'}`
      );
    },

    filterDaySelect(val, update) {
      this.filterSelect(val, this.daysAll, 'days', update);
    },

    filterMonthSelect(val, update) {
      this.filterSelect(val, this.monthsAll, 'months', update);
    },

    filterYearSelect(val, update) {
      this.filterSelect(val, this.yearsAll, 'years', update);
    },

    // Filter date select values based on input
    filterSelect(val, allOptions, propertyName, update) {
      update(
        // Filter select options only to those matching with val
        () => {
          val = val.trim();
          if (val === '') return (this[propertyName] = allOptions);

          const needle = val.toLowerCase();
          this[propertyName] = allOptions.filter((item) => {
            if (typeof item === 'object') {
              // If value matches, good to go
              if (this.compareOptionStrings(item.value, needle)) return true;

              // Compare if translated label matches
              if (item.label.includes('general')) {
                return this.compareOptionStrings(
                  this.$tr(item.label, null, false),
                  needle,
                );
              }
              return this.compareOptionStrings(item.label, needle);
            }

            if (typeof item === 'number') return this.compareOptionStrings(item.toString(), needle);

            return false;
          });
        },
        // After filtering, automatically focus first option
        (ref) => {
          if (val !== '' && ref.options.length > 0) {
            ref.setOptionIndex(-1); // reset optionIndex in case there is something selected
            ref.moveOptionSelection(1, true); // focus the first selectable option and do not update the input-value
          }
        },
      );
    },

    compareOptionStrings(str1, str2) {
      return str1.toLowerCase()
        .includes(str2);
    },
  },

  watch: {
    autofill() {
      const data = this.autofill;

      if (!data) return;

      // Move values from autofill prop to value variable
      for (const key in data) {
        // Birth date -> split day, month and year
        if (key === 'birthdate') {
          // Birthday is not null
          if (data[key]) {
            const value = data[key].split('-');
            this.values.birthYear = parseInt(value[0]);
            this.values.birthMonth = this.monthsAll[parseInt(value[1]) - 1];
            this.values.birthDay = this.daysAll[parseInt(value[2]) - 1];
          } else {
            this.values.birthDay = this.values.birthMonth = this.values.birthYear = null;
          }
        }
        // Diet -> pick correct value object
        else if (key === 'dietary_requirement') {
          this.values[key] = this.possibleDietsOptions.filter(
            (item) => item.value === data[key],
          )[0];
        }
        // Speaker status -> pick correct value object
        else if (key === 'speaker_status') {
          this.values[key] = this.speakerOptions.filter(
            (item) => item.value === data[key],
          )[0];
        } else if (key === 'gender') {
          this.values[key] = this.genderOptions.filter(
            (item) => item.value === data[key],
          )[0];
        } else if (key === 'school_year') {
          this.values['schoolYear'] = this.schoolYears.find(
            (item) => item.value === data[key]
          );
        }
        // Any other field -> pass raw value
        else {
          this.values[key] = data[key];
        }
      }
    },

    values: {
      handler() {
        this.$emit('update:model-value', this.submitData);
      },
      deep: true,
    },
  },

  computed: {
    submitData() {
      const returnObject = {
        name: this.values.name ? this.values.name.trim() : null,
        surname: this.values.surname ? this.values.surname.trim() : null,
        note: this.values.note,
        meals: this.values.meals,
        novice: this.values.novice,
        dietary_requirement: this.values.dietary_requirement
          ? this.values.dietary_requirement.value
          : null,
      };

      if (this.values.email || this.requireEmail) {
        returnObject.email = this.values.email;
      }

      if (this.values.schoolYear && this.requireSchoolYear) {
        returnObject.school_year = this.values.schoolYear.value;
      }

      // Include accommodation data if it is required or user wants it
      if (
        this.accommodationType === 'required' || (this.accommodationType !== 'none' && this.values.accommodation)
      ) {
        returnObject.accommodation = true;
        returnObject.birthdate = this.birthdateFormatter(
          this.values.birthYear,
          this.values.birthMonth,
          this.values.birthDay,
        );
        returnObject.id_number = (this.values.id_number !== '_________' && this.values.accommodation !== false) ? this.values.id_number : null;
        returnObject.street = this.values.street;
        returnObject.city = this.values.city;
        returnObject.zip = this.values.zip
          ? this.values.zip.replace(' ', '')
          : '';
      } else {
        returnObject.accommodation = false;
      }

      if (this.requireGender) {
        returnObject.gender = this.values.gender
          ? this.values.gender.value
          : null;
      }

      // PDS -> include speaker status
      if (this.requireSpeakerStatus) {
        returnObject.speaker_status = this.values.speaker_status
          ? this.values.speaker_status.value
          : null;
      }

      return returnObject;
    },
  },
};
</script>

<style scoped></style>
