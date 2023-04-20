module.exports = {
  root: true,
  env: {
    node: true,
  },
  extends: [
    'plugin:vue/vue3-essential',
    'eslint:recommended',
    'plugin:vue/base',
    'plugin:vuetify/base',
    '@vue/eslint-config-prettier/skip-formatting'
  ]
}
