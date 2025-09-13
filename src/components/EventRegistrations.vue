<template>
  <div class="q-pa-md">
    <q-table
      :rows="registrations"
      :columns="columns"
      :binary-state-sort="true"
      sort-by="surname"
      :pagination="initialPagination"
      :no-data-label="$tr('noData')"
      row-key="id"
      :filter="filterObject"
      :filter-method="filterTableRows"
      :loading="tableLoading"
      color="primary"
    >
      <!-- --- TABLE HEADER CELLS --- -->
      <!-- Role header cell - filterable -->
      <template v-slot:header-cell-role="props">
        <q-th :props="props" class="filterable-table-heading">
          <q-select borderless v-model="roleFilterModel"
                    :options="uniqueRoles(registrations)"
                    option-value="id"
                    :option-label="item => $tr(item.name, null, false)"
                    :label="props.col.label" :dense="true" :options-dense="true"
                    class="roles-filter" popup-content-class="table-filter-options">
            <template v-slot:prepend>
              <q-icon name="fas fa-filter" />
            </template>
          </q-select>
        </q-th>
      </template>
      <!-- Accommodation header cell - filterable -->
      <template v-slot:header-cell-accommodation="props">
        <q-th :props="props" class="filterable-table-heading">
          <q-select borderless v-model="accommodationFilterModel" :options="booleanFilterOptions"
                    :label="props.col.label" :dense="true" :options-dense="true"
                    class="accommodation-filter" popup-content-class="table-filter-options">
            <template v-slot:prepend>
              <q-icon name="fas fa-filter" />
            </template>
          </q-select>
        </q-th>
      </template>
      <!-- Meal type header cell - filterable -->
      <template v-slot:header-cell-meals="props">
        <q-th :props="props" class="filterable-table-heading">
          <q-select borderless v-model="mealsFilterModel" :options="booleanFilterOptions"
                    :label="props.col.label" :dense="true" :options-dense="true"
                    class="meals-filter" popup-content-class="table-filter-options">
            <template v-slot:prepend>
              <q-icon name="fas fa-filter" />
            </template>
          </q-select>
        </q-th>
      </template>
      <!-- Novice header cell - filterable -->
      <template v-slot:header-cell-novice="props">
        <q-th :props="props" class="filterable-table-heading">
          <q-select borderless v-model="noviceFilterModel" :options="booleanFilterOptions"
                    :label="props.col.label" :dense="true" :options-dense="true"
                    class="meals-filter" popup-content-class="table-filter-options">
            <template v-slot:prepend>
              <q-icon name="fas fa-filter" />
            </template>
          </q-select>
        </q-th>
      </template>

      <!-- --- TABLE BODY CELLS --- -->
      <!-- Role body cell -->
      <template v-slot:body-cell-role="props">
        <q-td :props="props">
          <!-- Admin view - show role select to edit -->
          <q-select borderless :model-value="editing?.role"
                    @update:model-value="(role) => editing.role = role"
                    :options="applicableRoles"
                    option-value="id" :option-label="item => $tr(item.name, null, false)"
                    :dense="true" :options-dense="true"
                    :disable="tableLoading"
                    v-if="editing?.id === props.row.id && type === 'admin'"/>
          <!-- Non-admin view - show static role -->
          <template v-else>
            {{ $tr(props.row.role.name) }}
          </template>
        </q-td>
      </template>
      <!-- Team body cell -->
      <template v-slot:body-cell-team="props">
        <q-td :props="props">
          <!-- Row is debater && admin view - show team select to edit -->
          <q-select borderless :model-value="editing?.team"
                    @update:model-value="(team) => editing.team = team"
                    :options="teams"
                    option-value="id" option-label="name"
                    :dense="true" :options-dense="true"
                    :disable="tableLoading"
                    v-if="editing?.id === props.row.id && type === 'admin'
                      && props.row.role.id ===  config.debaterRoleId" />
          <!-- Row is not debater || non-admin view - show static team -->
          <template v-else>
            {{ props.value }}
          </template>
        </q-td>
      </template>
      <!-- Note body cell -->
      <template v-slot:body-cell-note="props">
        <q-td :props="props" class="small-overflow-column">
          <q-input
            v-model="editing.note"
            outlined
            dense
            v-if="editing?.id === props.row.id && type === 'admin'"
          />
          <template v-else>
            {{ props.value }}
            <q-tooltip v-if="props.row.note">
              {{ props.value }}
            </q-tooltip>
          </template>
        </q-td>
      </template>
      <!-- Accommodation body cell -->
      <template v-slot:body-cell-accommodation="props">
        <q-td :props="props" class="small-overflow-column">
          <q-toggle
            v-model="editing.accommodation"
            v-if="editing?.id === props.row.id && type === 'admin'" />
          <template v-else>{{ props.value }}</template>
        </q-td>
      </template>
      <!-- Meals body cell -->
      <template v-slot:body-cell-meals="props">
        <q-td :props="props" class="small-overflow-column">
          <q-toggle
            v-model="editing.meals"
            v-if="editing?.id === props.row.id && type === 'admin'" />
          <template v-else>{{ props.value }}</template>
        </q-td>
      </template>
      <!-- Diet body cell -->
      <template v-slot:body-cell-dietary_requirements="props">
        <q-td :props="props">
          <!-- Admin view - show diet select to edit -->
          <q-select borderless :model-value="editing?.person.dietary_requirement"
                    @update:model-value="(diet) => editing.person.dietary_requirement = diet"
                    :options="diets"
                    option-value="id" :option-label="item => $tr(item.name)"
                    :dense="true" :options-dense="true"
                    :disable="tableLoading"
                    v-if="editing?.id === props.row.id && type === 'admin'">
            <template v-slot:option="scope">
              <q-item v-bind="scope.itemProps" :class="
                event.dietaryRequirements.find((dr) => dr.id === scope.opt.id)
                ? 'highlighted' : 'unhighlighted'
              ">
                <q-item-section>
                  <q-item-label>{{ scope.label }}</q-item-label>
                </q-item-section>
              </q-item>
            </template>
          </q-select>
          <!-- Non-admin view - show static diet -->
          <template v-else>
            {{ props.value }}
          </template>
        </q-td>
      </template>
      <!-- Novice body cell -->
      <template v-slot:body-cell-novice="props">
        <q-td :props="props" class="small-overflow-column">
          <q-toggle
            v-model="editing.novice"
            v-if="editing?.id === props.row.id && type === 'admin'" />
          <template v-else>{{ props.value }}</template>
        </q-td>
      </template>
      <!-- Editing body cell -->
      <template v-slot:body-cell-edit="props">
        <q-td :props="props">
          <q-btn round color="primary" icon="fas fa-pencil-alt" size="sm"
                 @click="startEditingRow(props.row)"
                 v-if="editing?.id !== props.row.id" />
          <q-btn round color="positive" icon="fas fa-save" size="sm"
                 @click="updateEditedData()"
                 v-else />
        </q-td>
      </template>
    </q-table>
  </div>
</template>

<script lang="ts">
import { DietaryRequirement, EventFull, EventRegistration } from 'src/types/event';
import { defineComponent } from 'vue';
import { Team } from 'src/types/debate';
import { Role } from 'src/types/role';
import { $tr } from 'boot/custom';
import config from 'src/config';
import { AxiosPromise, AxiosResponse } from 'axios';
import { getAllTranslations, TranslatedString } from 'boot/i18n';
import { langs } from 'src/translation/config';
import { EventsRegistrationsObjectType } from 'src/store/eventsRegistrations/state';
import { customCompareStrings } from 'src/boot/custom';

const booleanFilterOptions = [$tr('event.registrationsOverview.all'), $tr('event.registrationsOverview.yes'), $tr('event.registrationsOverview.no')];

const fakeRoleObject = (nameTrKey: string): Role => ({
  id: Infinity,
  icon: '',
  name: {
    id: Infinity,
    created_at: '',
    updated_at: '',
    ...getAllTranslations(nameTrKey),
  },
  slug: <TranslatedString><unknown> new Map(langs.map((lang) => ([lang, '']))),
  created_at: '',
  updated_at: '',
});

export const uniqueRoles = (registrations: EventRegistration[]): Role[] => {
  if (!registrations) {
    return [];
  }

  const roles: Role[] = (Object.values(registrations))
    .map((item) => item.role);
  const idsOnly = roles.map((item) => item.id);
  return [
    fakeRoleObject('event.registrationsOverview.all'),
    ...roles.filter((item, index) => idsOnly.indexOf(item.id) === index),
  ];
};

interface FilterObject {
  role: Role | null;
  accommodation: typeof booleanFilterOptions[number] | null;
  meals: (typeof booleanFilterOptions[number]) | null;
}

export default defineComponent({
  name: 'EventRegistrations',
  props: {
    event: {
      type: Object as () => EventFull,
      required: true,
    },
    type: String as () => EventsRegistrationsObjectType,
  },
  computed: {
    registrations(): EventRegistration[] {
      // eslint-disable-next-line max-len
      // eslint-disable-next-line @typescript-eslint/no-unsafe-call,@typescript-eslint/no-unsafe-member-access
      return <EventRegistration[]> this.$store.getters['eventsRegistrations/eventRegistrations'](this.event.id, this.type);
    },
    filterObject(): FilterObject {
      return {
        role: this.roleFilterModel,
        accommodation: this.accommodationFilterModel,
        meals: this.mealsFilterModel,
      };
    },
    participantRoles(): Record<number, Role> {
      return Object.fromEntries(
        new Map(this.registrations.map(
          (registration: EventRegistration) => ([registration.id, registration.role]),
        )),
      );
    },
    applicableRoles(): Role[] {
      return [
        this.fakeRoleObject('event.types.organizer'),
        // eslint-disable-next-line max-len
        // eslint-disable-next-line @typescript-eslint/no-unsafe-call,@typescript-eslint/no-unsafe-member-access
        ...<Role[]> this.$store.getters['roles/eventRoles'](this.event.id),
      ];
    },
    teams(): Team[] {
      // eslint-disable-next-line max-len
      // eslint-disable-next-line @typescript-eslint/no-unsafe-call,@typescript-eslint/no-unsafe-member-access
      return <Team[]> this.$store.getters['eventsTeams/eventTeamsSimple'](this.event.id) ?? [];
    },
    diets(): DietaryRequirement[] {
      // eslint-disable-next-line max-len
      // eslint-disable-next-line @typescript-eslint/no-unsafe-member-access,@typescript-eslint/no-explicit-any
      return <DietaryRequirement[]> (<any> this.$store.state).diets.diets;
    },
  },
  async created() {
    // Not cached -> load from API
    await Promise.all(
      [
        this.$store.dispatch('events/loadFull', this.event.id), // for roles
        this.$store.dispatch('roles/load'),
        this.$store.dispatch('eventsRegistrations/load', [this.event.id, this.type]),
      ].concat(
        (this.type === 'admin' ? [
          this.$store.dispatch('eventsTeams/loadSimple', this.event.id),
          this.$store.dispatch('diets/load'),
        ] : []),
      ),
    );
  },
  data() {
    const outputBoolean = (val: boolean) => (val ? '✅' : '❌');
    const emptyToHyphen = (val: string | null) => (val ?? '-');
    const dietOrHyphen = (diet: DietaryRequirement | null) => (diet ? this.$tr(diet.name) : '-');

    const columns: unknown[] = [{
      name: 'surname', label: this.$tr('event.registrationsOverview.labels.surname'), field: (row: EventRegistration) => row.person.surname, sortable: true, sort: (a: string, b: string) => customCompareStrings(a, b), align: 'left',
    }, {
      name: 'name', label: this.$tr('event.registrationsOverview.labels.name'), field: (row: EventRegistration) => row.person.name, sortable: true, sort: (a: string, b: string) => customCompareStrings(a, b), align: 'left',
    }, {
      name: 'role', label: this.$tr('event.registrationsOverview.labels.role'), sortable: false, align: 'left',
    }, {
      name: 'team', label: this.$tr('event.registrationsOverview.labels.team'), field: (row: EventRegistration) => row.team?.name ?? '-', sortable: true, sort: (a: string, b: string) => customCompareStrings(a, b), align: 'left',
    }, {
      name: 'note', label: this.$tr('event.registrationsOverview.labels.note'), field: 'note', format: emptyToHyphen, sortable: true, sort: (a: string, b: string) => customCompareStrings(a, b), align: 'left',
    }];

    if (this.event.parent_email_required) {
      columns.push({
        name: 'parent_email', label: this.$tr('event.registrationsOverview.labels.parentEmail'), field: (row: EventRegistration) => row.person.parent_email, format: emptyToHyphen, sortable: true, sort: (a: string, b: string) => customCompareStrings(a, b), align: 'left',
      });
    }

    if (this.event.accommodation !== 'none') {
      columns.push({
        name: 'accommodation', label: this.$tr('event.registrationsOverview.labels.accommodation'), field: 'accommodation', format: outputBoolean, sortable: false, align: 'center',
      });
    }

    if (this.event.meals !== 'none') {
      columns.push({
        name: 'meals', label: this.$tr('event.registrationsOverview.labels.meals'), field: 'meals', format: outputBoolean, sortable: false, align: 'center',
      });
      columns.push({
        name: 'dietary_requirements', label: this.$tr('event.registrationsOverview.labels.dietaryRequirements'), field: (row: EventRegistration) => row.person.dietary_requirement, format: dietOrHyphen, sortable: true, sort: (a: string, b: string) => customCompareStrings(a, b), align: 'center',
      });
    }

    if (this.event.novices) {
      columns.push({
        name: 'novice', label: this.$tr('event.registrationsOverview.labels.novice'), field: 'novice', format: outputBoolean, sortable: false, align: 'center',
      });
    }

    if (this.type === 'admin') {
      columns.push({
        name: 'edit', label: '',
      });
    }

    return {
      config,
      translationPrefix: 'event.registrationsOverview.',
      roleFilterModel: null,
      accommodationFilterModel: null,
      mealsFilterModel: null,
      noviceFilterModel: null,
      editing: {},
      booleanFilterOptions,
      columns,
      initialPagination: {
        sortBy: 'surname',
        descending: false,
        rowsPerPage: 20,
      },
      tableLoading: false,
    };
  },
  methods: {
    filterTableRows(rows: EventRegistration[], terms: FilterObject): EventRegistration[] {
      return rows.filter((item) => (
        (terms.role == null || terms.role.id === Infinity || terms.role.id === item.role.id)
        && (terms.accommodation == null || terms.accommodation === this.$tr('all') || ((terms.accommodation === this.$tr('yes')) === item.accommodation))
        && (terms.meals == null || terms.meals === this.$tr('all') || ((terms.meals === this.$tr('yes')) === item.meals))
      ));
    },
    startEditingRow(row: EventRegistration) {
      this.editing = {
        id: row.id,
        note: row.note,
        role: row.role,
        team: row.team,
        accommodation: row.accommodation,
        meals: row.meals,
        novice: row.novice ?? false,
        person: {
          dietary_requirement: row.person.dietary_requirement ?? this.diets.find(
            (diet) => diet.id === config.noDietaryRequirementId,
          ),
        },
      };
    },
    updateEditedData() {
      const updatedData: EventRegistration = <EventRegistration> this.editing;

      if (!updatedData?.id) {
        return;
      }

      this.tableLoading = true;

      void this.updateDietaryRequirements(
        this.registrations.find((r) => r.id === updatedData.id)!.person.id,
        updatedData.person.dietary_requirement!.id,
      )
        .then(() => {
          this.$api({
            url: `registration/${updatedData.id}`,
            method: 'put',
            data: {
              ...updatedData,
              role: updatedData.role.id === Infinity ? 4 : updatedData.role.id,
              team: updatedData.team?.id ?? null,
              person: undefined,
            },
          })
            .then(({ data }: AxiosResponse<EventRegistration>) => {
              this.editing = {};
              this.$store.commit('eventsRegistrations/updateEventRegistration', {
                eventId: this.event.id,
                data,
              });

              // Invalidate event teams for admin teams view
              this.$store.commit('eventsTeams/flushEventTeamsDetailed', this.event.id);
            })
            .finally(() => {
              this.tableLoading = false;
            });
        });
    },
    updateDietaryRequirements(personId: number, value: number): AxiosPromise {
      return this.$api({
        url: `person/${personId}`,
        method: 'put',
        data: {
          dietary_requirement: value,
        },
      });
    },
    uniqueRoles,
    fakeRoleObject,
  },
});

</script>
