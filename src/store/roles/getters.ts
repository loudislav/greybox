import { Role } from 'src/types/role';
import { RolesState } from 'src/store/roles/state';
import { Getter } from 'vuex';
import { State } from 'src/store';
import { EventFull, EventPrice } from 'src/types/event';
import { $organizer } from 'boot/custom';
import { translationMatchesInAnyLanguage } from 'boot/i18n';

export const role = (state: RolesState) => (id: number): Role | undefined => state.roles
  .find((item) => (item.id === id));

export const eventRoles: Getter<RolesState, State> = (
  state: RolesState, _, __, rootGetters,
) => (eventId: number, isIndividual: boolean = true): Role[] => {
  // eslint-disable-next-line max-len
  // eslint-disable-next-line @typescript-eslint/no-unsafe-call,@typescript-eslint/no-unsafe-assignment,@typescript-eslint/no-unsafe-member-access
  const event: EventFull = rootGetters['events/fullEvent'](eventId);

  const rolePrice = (roleId: number): EventPrice | undefined => event.prices
    .find((price: EventPrice) => price.role.id === roleId);

  // Check if roles are present in event's prices
  return state.roles
    .filter((r: Role) => (
      (
        r.id === Infinity // Is team role...
      && !isIndividual // ... registration type is not individual...
      && rolePrice(1) // ...debater role is present
      ) || (
        rolePrice(r.id) // Role is present in pricing...
      && (!$organizer || r.id !== 1) // ...and is not an individual debater on PDS
      )
    ))
    .map((r: Role) => ({
      ...r,
      note: rolePrice(r.id)?.note ?? null,
    }));
};

export const roleFromSlug = (state: RolesState) => (
  slug: string,
): Role | undefined => state.roles.find((r: Role) => translationMatchesInAnyLanguage(r.slug, slug));
