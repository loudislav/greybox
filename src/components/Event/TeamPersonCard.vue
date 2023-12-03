<template>
  <div>
    <q-card flat bordered class="team-person-card bg-grey-1 q-mb-md">
      <q-card-section class="header" @click="$emit('toggleVisibility', id)">
        <div class="row items-center no-wrap">
          <div class="col">
            <q-btn
              color="grey-7"
              round
              flat
              :icon="'fas fa-' + (visible ? 'minus' : 'plus')"
              class="float-left"
              size="sm"
            />
            <div class="text-h6">
              <template v-if="formData.name || formData.surname">
                {{ formData.name }} {{ formData.surname }}
              </template>
              <template v-else>
                {{ $tr("types.debater") }} {{ index + 1 }}
              </template>
            </div>
          </div>

          <div class="col-auto">
            <q-icon
              name="fas fa-exclamation-circle"
              class="text-negative"
              v-if="error"
            />
            <q-btn
              color="grey-7"
              round
              flat
              icon="fas fa-trash"
              size="sm"
              @click="$emit('delete', id)"
            />
          </div>
        </div>
      </q-card-section>
      <div class="q-card-section-wrapper" :class="{ hiddenCard: !visible }">
        <q-card-section class="" >
          <form-fields
              ref="form-fields"
              @update:model-value="catchInput"
              :autofill="autofill"
              :is-team="true"
              :accommodationType="accommodationType"
              :mealType="mealType"
              :novices="novices"
              :possibleDiets="possibleDiets"
              :role="1"
              :requireEmail="requireEmail"
              :requireGender="requireGender"
          />
        </q-card-section>
      </div>
    </q-card>
  </div>
</template>

<script>
/* eslint-disable */
import formFields from './FormFields';

export default {
  name: 'TeamPersonCard',
  components: {
    formFields,
  },
  props: {
    autofill: Object,
    id: [String, Number],
    index: Number,
    visible: Boolean,
    error: Boolean,
    accommodationType: String,
    mealType: String,
    novices: Boolean,
    possibleDiets: Array,
    requireEmail: Boolean,
    requireGender: Boolean,
  },
  emits: [
    'toggleVisibility',
    'update:model-value',
    'delete',
  ],
  data() {
    return {
      translationPrefix: 'event.',
      formData: {},
    };
  },
  methods: {
    catchInput(data) {
      this.formData = data;
      this.$emit('update:model-value', data, this.id);
    },
    updateHeight() {
      /* TODO
      //console.log(elem);
      //let el = document.querySelectorAll('.q-card-section-wrapper');
      let height = this.$el.scrollHeight;
      this.$el.style('height', height + 'px')
      setTimeout(() => {
        //this.$el.style('height', 'auto')
        console.log(this.$el);
      }, 500);
       */
    }
  },
  watch: {
    visible: function (val) {
      this.updateHeight()
    }
  }

};
</script>
