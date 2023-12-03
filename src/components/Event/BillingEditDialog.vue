<template>
  <q-dialog :model-value="visible" @update:model-value="stateChange">
    <q-card class="dialog-small">
      <q-card-section class="row items-center">
        <div class="text-h6">
          {{ $tr('modal.title.' + (this.client ? 'edit' : 'add')) }}
        </div>
        <q-space />
        <q-btn icon="fas fa-times" flat round dense v-close-popup />
      </q-card-section>

      <q-form ref="q-form" @submit="submitForm" v-show="initialized">
        <q-card-section>
          <div class="row q-col-gutter-md q-pb-sm">
            <q-input
              outlined
              :label="$tr('fields.name') + ' *'"
              class="col-12 q-pb-md"
              lazy-rules
              v-model="values.name"
              :rules="[$validators.nonEmpty]"
            >
              <template v-slot:prepend>
                <q-icon name="fas fa-user" />
              </template>
            </q-input>
            <q-input
              outlined
              :label="$tr('fields.email')"
              class="col-12"
              v-model="values.email"
            >
              <template v-slot:prepend>
                <q-icon name="fas fa-at" />
              </template>
            </q-input>
            <q-input
              v-model="values.registration_no"
              outlined
              :label="$tr('fields.registration_no')"
              class="col-12"
            >
              <template v-slot:prepend>
                <q-icon name="fas fa-hashtag" />
              </template>
            </q-input>

            <q-input
              v-model="values.street"
              outlined
              :label="$tr('fields.street')"
              class="col-12"
              :input-class="
                'smartform-street-and-number ' + 'smartform-instance-' + uuid
              "
              lazy-rules
            >
              <template v-slot:prepend>
                <q-icon name="fas fa-home" />
              </template>
            </q-input>

            <q-input
              v-model="values.city"
              outlined
              :label="$tr('fields.city')"
              class="col-7"
              :input-class="'smartform-city ' + 'smartform-instance-' + uuid"
              lazy-rules
              hint=""
            >
              <template v-slot:prepend>
                <q-icon name="fas fa-city" />
              </template>
            </q-input>

            <template v-if="$organizer === 'pds'">
              <mask-input
                  v-model="values.zip"
                  outlined
                  :label="$tr('fields.zip')"
                  class="col-5"
                  :input-class="'smartform-zip ' + 'smartform-instance-' + uuid"
                  :mask="validateZip ? '### ##' : ''"
                  fill-mask="_"
                  :hint="validateZip ? $tr('fieldNotes.example') + ' 796 01' : null"
              >
                <template v-slot:prepend>
                  <q-icon name="fas fa-file-archive" />
                </template>
              </mask-input>
            </template>
            <template v-else>
              <mask-input
                  v-model="values.zip"
                  outlined
                  :label="$tr('fields.zip')"
                  class="col-5"
                  :input-class="'smartform-zip ' + 'smartform-instance-' + uuid"
                  :mask="validateZip ? '### ##' : ''"
                  fill-mask="_"
                  :hint="validateZip ? $tr('fieldNotes.example') + ' 796 01' : null"
                  lazy-rules
              >
                <template v-slot:prepend>
                  <q-icon name="fas fa-file-archive" />
                </template>
              </mask-input>
            </template>

          </div>
          <country-select v-if="$isPDS" v-model="values.country" />
        </q-card-section>
        <q-card-actions class="float-actions">
          <q-btn
            :label="$tr('removeButton')"
            color="red"
            flat
            :class="{ hide: !client }"
            @click="removeClient"
          />
          <q-btn
            :label="$tr('general.save', null, false)"
            type="submit"
            color="primary"
          />
        </q-card-actions>
      </q-form>
    </q-card>
  </q-dialog>
</template>

<script>
/* eslint-disable */
import MaskInput from './MaskInput';
import CountrySelect from './CountrySelect';

export default {
  name: 'BillingEditDialog',
  components: {
    CountrySelect,
    MaskInput
  },
  props: {
    visible: Boolean,
    client: Object,
  },
  emits: [
    'state-change',
    'client-change',
  ],
  data() {
    return {
      translationPrefix: 'event.checkout.billing.',
      initialized: false,
      values: {
        name: null,
        email: null,
        registration_no: null,
        street: null,
        city: null,
        zip: null,
        country: null,
      },
    };
  },
  created() {
    // Smartform autocomplete select
    this.$bus.$on('smartform', (data) => {
      // If instance ID is this form
      if (data.instance.substr(-(this.uuid.length)) == this.uuid) this.values[data.field] = data.value;
    });

    this.stateChange(this.visible);
  },
  computed: {
    validateZip() {
      return (
        this.$organizer === 'adk'
        || (this.values.country && this.values.country.value === 'CZ')
      );
    },
  },
  methods: {
    stateChange(isVisible) {
      if (isVisible) this.$nextTick(this._dialogMounted);

      return this.$emit('state-change', isVisible);
    },

    _dialogMounted() {
      Object.keys(this.values)
        .forEach((key) => {
          this.values[key] = this.client && this.client[key] ? this.client[key] : null;
        });

      this.$nextTick(this._initSmartform);
      this.initialized = true;
    },

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

    submitForm() {
      this.$bus.$emit('fullLoader', true);

      const isEdit = !!this.client;

      const data = { ...this.values };

      // Remove mask from zip
      if (this.validateZip) {
        data.zip = data.zip
          ? data.zip
            .replace(/_/g, '')
            .replace(' ', '')
            .trim()
          : '';
      }

      if (this.$organizer !== 'adk' && data.country) data.country = data.country.value;

      this.$api({
        url: `client${isEdit ? `/${this.client.id}` : ''}`,
        method: isEdit ? 'put' : 'post',
        data,
        alerts: false,
      })
        .then((data) => {
          this.stateChange(false);
          this.$flash(
            this.$tr(`success.${isEdit ? 'edit' : 'add'}`),
            'success',
          );

          const client = data.data;

          this.$emit('client-change', client.id, client, !isEdit);
        })
        .catch((data) => {
          if (data.response.data) {
            const { message } = data.response.data;

            if (message && this.$trExists(`validation.${message}`, null, false)) return this.$flash(this.$tr(`validation.${message}`, null, false), 'error');
          }
          this.$flash(this.$tr(`error.${isEdit ? 'edit' : 'add'}`), 'error');
        })
        .finally(() => {
          this.$bus.$emit('fullLoader', false);
        });
    },

    removeClient() {
      if (!this.client) return false;

      this.$confirm({
        confirm: this.$tr('general.confirmModal.remove', null, false),
        message: this.$tr('removeModal.title'),
      })
        .onOk(() => {
          this.$bus.$emit('fullLoader', true);

          this.$api({
            url: `client/${this.client.id}`,
            method: 'delete',
            alerts: false,
          })
            .then(() => {
              this.stateChange(false);
              this.$flash(this.$tr('success.delete'), 'success');

              this.$emit('client-change', this.client.id, null);
            })
            .catch(() => {
              this.$flash(this.$tr('error.delete'), 'error');
            })
            .finally(() => {
              this.$bus.$emit('fullLoader', false);
            });
        });
    },
  },
};
</script>
