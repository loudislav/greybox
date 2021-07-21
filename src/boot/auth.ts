import { boot } from 'quasar/wrappers';
import { useRouter } from 'vue-router';
import apiCall from 'src/api';
import { AxiosResponse } from 'axios';

type UserRole = 'admin' | 'none';

interface Person {
  name: string
  surname: string
  email: string
  // eslint-disable-next-line camelcase
  school_year: number
  birthdate: string
  // eslint-disable-next-line camelcase
  id_number: string
  street: string
  city: string
  zip: string
  // eslint-disable-next-line camelcase
  dietary_requirement: number
  // eslint-disable-next-line camelcase
  speaker_status: string
  note: string
  // eslint-disable-next-line camelcase
  updated_at: string
  // eslint-disable-next-line camelcase
  created_at: string
  id: number
}

interface User {
  admin: boolean
  // eslint-disable-next-line camelcase
  api_token: string
  // eslint-disable-next-line camelcase
  created_at: string
  id: 292
  // eslint-disable-next-line camelcase
  id_token: string
  person: Person | null
  // eslint-disable-next-line camelcase
  person_id: number | null
  // eslint-disable-next-line camelcase
  preferred_locale: string
  role: UserRole
  // eslint-disable-next-line camelcase
  updated_at: string
  username: string
}

export interface LoginData {
  username: string
  password: string
  isSignUp: boolean
}

interface Auth {
  check: () => boolean
  getToken: () => string | null
  user: () => User | null,
  logout: () => void
  login: (data: LoginData) => Promise<User | null>
  isAdmin: () => boolean
}

const localStorageKey = 'greyboxAuthData';

const user = (): User | null => {
  const data = localStorage.getItem(localStorageKey);
  if (!data) return null;
  // eslint-disable-next-line @typescript-eslint/no-unsafe-return
  return JSON.parse(data);
};

const getToken = (): string | null => user()?.api_token ?? null;

const check = (): boolean => getToken() !== null;

const logout = async () => {
  localStorage.removeItem(localStorageKey);
  const router = useRouter();
  await router.replace({ name: 'home' });
};

const login = (credentials: LoginData): Promise<User | null> => new Promise(
  (resolve, reject) => {
    const {
      username,
      password,
    } = credentials;
    apiCall({
      url: 'login',
      sendToken: false,
      data: {
        username,
        password,
      },
    })
      .then((response: AxiosResponse<User>) => {
        const { data } = response;
        localStorage.setItem(localStorageKey, JSON.stringify(data));
        resolve(data);
      })
      .catch(reject);
  },
);

export const isAdmin = (): boolean => check() && user()?.role === 'admin';

const auth: Auth = {
  check,
  getToken,
  user,
  logout,
  login,
  isAdmin,
};

export default boot(({ app }) => {
  app.config.globalProperties.$auth = auth;
});

export { auth };