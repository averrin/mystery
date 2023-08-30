<script>
  import Icon from "@iconify/svelte";
  import { Tooltip, Badge } from "flowbite-svelte";
  export let location;
  import cityStore from "../stores";

  let building = $cityStore.geo.buildings[location.building];
  let street = $cityStore.geo.streets[building.street];

  let color = "#eee";
  if (building.building_type == "residential") {
    // color = "#f44336";
  } else if (building.building_type == "commerical") {
    color = "blue";
  } else if (building.building_type == "industrial") {
    color = "yellow";
  } else if (building.building_type == "other") {
    color = "green";
  }
  if (location.name == "Empty") {
    color = "dark";
  }
</script>

{#if location}
  <Badge border large={true} {color}>
    <span
      id={`badge-location-${location.id}`}
      class="flex flex-row gap-2 items-center"
    >
      <Icon icon="carbon:location-filled" {color} />

      {#if location.name}
        <span class="text-md">{location.name} | </span>
      {/if}

      {street.name}
      <b
        >{building.number}
        {#if location.apartment}
          {#if building.building_type.startsWith("residential")}
            apt.
          {:else}
            room.
          {/if}
          {location.apartment}
        {/if}
      </b>
    </span></Badge
  >
  <Tooltip type={"dark"} triggeredBy={`#badge-location-${location.id}`}>
    ID: {location.id} | Type: {building.building_type}
  </Tooltip>
{/if}
