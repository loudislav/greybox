import { RouteRecordRaw } from 'vue-router';
import { adminMiddleware, loggedInMiddleware, notLoggedInMiddleware } from './middlewares';

// Translations
import CZroutes from '../translation/cs/paths.json';
import ENroutes from '../translation/en/paths.json';

const routes: RouteRecordRaw[] = [
  {
    path: '/',
    component: () => import('layouts/Default.vue'),
    children: [
      {
        path: CZroutes.home,
        alias: ENroutes.home,
        name: 'home',
        component: () => import('pages/Home.vue'),
      },
      {
        path: CZroutes.about,
        alias: ENroutes.about,
        name: 'about',
        component: () => import('pages/About.vue'),
      },
      {
        path: `${CZroutes.tournament}/:id-:slug`,
        alias: `${ENroutes.tournament}/:id-:slug`,
        name: 'tournament',
        component: () => import('pages/Event.vue'),
      },

      // Admin
      {
        path: CZroutes.admin.eventRegistrations,
        alias: ENroutes.admin.eventRegistrations,
        name: 'event-registrations',
        component: () => import('pages/Admin/EventRegistrations.vue'),
        beforeEnter: adminMiddleware,
      },

      // Auth
      {
        path: CZroutes.auth.login,
        alias: ENroutes.auth.login,
        name: 'login',
        component: () => import('pages/Auth/Login.vue'),
        props: true,
        beforeEnter: notLoggedInMiddleware,
      },
      {
        path: CZroutes.auth.signUp,
        alias: ENroutes.auth.signUp,
        name: 'sign-up',
        component: () => import('pages/Auth/SignUp.vue'),
        beforeEnter: notLoggedInMiddleware,
      },
      {
        path: CZroutes.auth.logout,
        alias: ENroutes.auth.logout,
        name: 'logout',
        component: () => import('pages/Auth/Logout.vue'),
        beforeEnter: loggedInMiddleware,
      },
      {
        path: CZroutes.auth.passwordReset,
        alias: ENroutes.auth.passwordReset,
        name: 'password-reset',
        component: () => import('pages/Auth/PasswordReset.vue'),
        beforeEnter: notLoggedInMiddleware,
      },
      {
        path: `${CZroutes.auth.passwordReset}/:token`,
        alias: `${ENroutes.auth.passwordReset}/:token`,
        name: 'new-password',
        component: () => import('pages/Auth/NewPassword.vue'),
        beforeEnter: notLoggedInMiddleware,
      },
      {
        path: CZroutes.auth.accountSettings,
        alias: ENroutes.auth.accountSettings,
        name: 'account-settings',
        component: () => import('pages/Auth/AccountSettings.vue'),
        beforeEnter: loggedInMiddleware,
      },
    ],
  },

  // Always leave this as last one,
  // but you can also remove it
  {
    path: '/:catchAll(.*)*',
    component: () => import('layouts/Default.vue'),
    children: [{
      path: '',
      component: () => import('pages/404.vue'),
    }],
  },
];

export default routes;
