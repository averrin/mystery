<script>
  import { Tooltip, Badge } from "flowbite-svelte";

  import Chip from "./Chip.svelte";
  export let group;
  import cityStore from "../stores";

  function stringToColor(str) {
    let hash = 0;
    str.split("").forEach((char) => {
      hash = char.charCodeAt(0) + ((hash << 5) - hash);
    });
    let colour = "#";
    for (let i = 0; i < 3; i++) {
      const value = (hash >> (i * 8)) & 0xff;
      colour += value.toString(16).padStart(2, "0");
    }
    return colour;
  }
  let color = stringToColor(`${group.id} ${group.name}`);
</script>

{#if group}
  {@const roles = group.roles.map((r) => $cityStore.roles[r])}
  <span id={`badge-group-${group.id}`}>
    <Chip icon="wpf:group" iconColor={color} color="#ffffcc">
      {group.name}
    </Chip>
  </span>
  <Tooltip type={"dark"} triggeredBy={`#badge-group-${group.id}`}
    >Roles: {roles.length} | Members: {roles.reduce(
      (acc, r) => acc + r.members.length,
      0
    )}</Tooltip
  >
{/if}
