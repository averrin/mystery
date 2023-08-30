<script>
  import Icon from "@iconify/svelte";
  import {
    Tooltip,
    Badge,
  } from "flowbite-svelte";
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
  <Badge border large={true} color="dark">
    <span
      id={`badge-group-${group.id}`}
      class="flex flex-row gap-2 items-center"
    >
      <Icon icon="wpf:group" {color} />
      {group.name}
    </span></Badge
  >
  <Tooltip type={"dark"} triggeredBy={`#badge-group-${group.id}`}
    >Roles: {roles.length} | Members: {roles.reduce(
      (acc, r) => acc + r.members.length,
      0
    )}</Tooltip
  >
{/if}
