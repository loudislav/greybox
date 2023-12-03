import { Action } from 'vuex';
import { bus } from 'boot/eventBus';
import { AxiosResponse } from 'axios';
import { $organizer, $makeIdObject } from 'boot/custom';
import { apiCall } from 'boot/api';
import { Event, EventFull } from 'src/types/event';
import { EventsState } from 'src/store/events/state';
import { user } from 'boot/auth';

export const load: Action<EventsState, never> = async ({
  commit,
  state,
}, [loading = true, forceReload = false]: boolean[] = []) => {
  if ((state.loaded && !forceReload) || state.loading) {
    return;
  }

  commit('startLoadingEvents');

  if (loading) {
    bus.$emit('fullLoader', true);
  }

  await apiCall({
    url: 'event',
    sendToken: false,
    method: 'get',
  })
    .then(({ data }: AxiosResponse<Event[]>) => {
      commit('setEvents', $makeIdObject(
        data
          .filter((event) => event.organizer === $organizer)
          .map((item) => ({
            ...item,
            fullyLoaded: false,
          })),
      ));
    })
    .finally(() => {
      if (loading) {
        bus.$emit('fullLoader', false);
      }
    });
};

export const loadFull: Action<EventsState, never> = async ({
  commit,
  state,
}, id: number) => {
  if (state.eventsFull[id]) {
    return;
  }

  bus.$emit('fullLoader', true);

  await apiCall({
    url: `event/${id}`,
    method: 'get',
  })
    .then(({ data: event }: AxiosResponse<EventFull>) => {
      commit('setFullEvent', event);
    })
    .finally(() => {
      bus.$emit('fullLoader', false);
    });
};

export const loadAll: Action<EventsState, never> = async ({
  commit,
  state,
}, loading: boolean = true) => {
  if (state.loadedAll || state.loadingAll) {
    return;
  }

  commit('startLoadingAllEvents');

  if (loading) {
    bus.$emit('fullLoader', true);
  }

  await apiCall({
    url: `user/${user()!.id}/event`,
    method: 'get',
  })
    .then(({ data }: AxiosResponse<Event[]>) => {
      if (!data.length) {
        return;
      }

      commit('setAllEvents', data.filter((event) => event.organizer === $organizer));
    })
    .finally(() => {
      if (loading) {
        bus.$emit('fullLoader', false);
      }
    });
};
