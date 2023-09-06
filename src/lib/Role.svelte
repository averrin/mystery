<script lang="ts">
  import { Badge } from "flowbite-svelte";
  import cityStore from "../stores";
  import GroupName from "./GroupName.svelte";

  import Chip from "./Chip.svelte";
  export let role;

  function cap(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
  }
</script>

<Chip color="#ccc">
  <b>{cap(role.name)}</b>
  @
  <GroupName group={$cityStore.groups[role.group_id]} />
</Chip>
