# greybox

## Project setup
```
npm install
```
## Environments
This project runs in 3 environments - development, debug and production.

- **Development** should be used for local development only
- **Debug** should be used for development version of the server
- **Production** should be used for live version of the server

Main difference is that only **production** has Vue devtools turned off and uses live API.

Configuration can be changed in `.env.[mode]` files in root folder. Current mode will be visible next to logo in navbar (except for production).

### Compiles and hot-reloads for development
```
npm run serve
```

This command is in **development** mode by default. Use `npm run serve:debug` to start local server with debug mode on `serve:prod` for production.

### Compiles and minifies for production
```
npm run build
```

This command builds **production** mode by default. Use `npm run build:debug` to build debug version or `npm run build:dev` for development.

### Run your tests
```
npm run test
```

### Lints and fixes files
```
npm run lint
```

### Customize configuration
See [Configuration Reference](https://cli.vuejs.org/config/).

## Deployment
This project is ready to be deployed on apache webserver with history router mode. To change the directory of the app, you need to edit it in `vue.config.js:2`, `src/router.js:19` and `public/.htaccess:7` and rebuild.

History hash mode is used for development to make things easier.