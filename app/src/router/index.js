// Composables
import { createRouter, createWebHistory } from 'vue-router'

// Pages and components
import Layout from '@/components/Layout.vue';
import Light from "@/pages/Light.vue";
import Settings from "@/pages/Settings.vue";

const routes = [
  {
    path: '/',
    redirect: 'home',
    name: 'Layout',
    component: Layout,
    children: [
      {
        path: 'home',
        name: 'Home',
        component: Light,
      },
      {
        path: 'settings',
        name: 'Settings',
        component: Settings,
      },
    ]
  }
]

const router = createRouter({
  history: createWebHistory(process.env.BASE_URL),
  routes,
})

export default router
