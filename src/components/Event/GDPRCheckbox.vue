<template>
  <div>
    <q-checkbox
      :model-value="modelValue"
      @update:model-value="emitChange"
      :class="{ 'q-field--error text-bold': error && !modelValue }"
    >
      {{ $tr("label") }}
      <a @click="showModal = true" ref="labelLink">{{ $tr("link") }}</a>
    </q-checkbox>
    <q-dialog v-model="showModal">
      <q-card class="dialog-medium">
        <q-card-section class="row items-center">
          <div class="text-h6">
            {{ $tr("modal.title") }}
          </div>
          <q-space />
          <q-btn icon="fas fa-times" flat round dense v-close-popup />
        </q-card-section>

        <q-card-section>
          {{ $tr("modal.opening") }}
          <ul>
            <li
              v-for="item in $tr('modal.list' + ($organizer !== 'adk' ? 'International' : ''))"
              v-bind:key="item"
            >
              {{ item }}
            </li>
          </ul>
          {{ $tr("modal.closing") }}
        </q-card-section>
      </q-card>
    </q-dialog>
  </div>
</template>

<script>
/* eslint-disable */
export default {
  name: 'GDPRCheckbox',
  props: {
    modelValue: Boolean,
    error: Boolean,
  },
  emits: [
    'update:model-value'
  ],
  data() {
    return {
      translationPrefix: 'event.gdpr.',
      showModal: false,
    };
  },
  methods: {
    emitChange(a) {
      this.$emit('update:model-value', a);
    },
  },
  mounted() {
    // Remove label toggling on inner link click
    this.$refs.labelLink.addEventListener('click', (e) => {
      e.stopPropagation();
      e.preventDefault();
    });
  },
  beforeUnmount() {
    this.$refs.labelLink.removeEventListener('click', (e) => {
      e.stopPropagation();
      e.preventDefault();
    });
  },
};
</script>
