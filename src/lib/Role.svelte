<script lang="ts">
  import { Badge } from "flowbite-svelte";
  import cityStore from "../stores";
  import GroupName from "./GroupName.svelte";
  export let role;

  function cap(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
  }
</script>

<Badge border large={true} color="dark">
  <span class="flex flex-row gap-2 items-center">
    <b>{cap(role.name)}</b>
    @
    <GroupName group={$cityStore.groups[role.group_id]} />
  </span>
</Badge>
