<script>
  import Icon from "@iconify/svelte";
  import { Tooltip, Badge } from "flowbite-svelte";
  export let location;
  import cityStore from "../stores";

  import Chip from "./Chip.svelte";

  let building = $cityStore.geo.buildings[location.building];
  let street = $cityStore.geo.streets[building.street];

  let icon = "carbon:location-filled";
  let color = "#aaa";
  if (location.location_type == "building") {
    if (building?.building_type?.startsWith("residential")) {
      icon = "fa6-solid:house";
    } else if (building.building_type == "commercial") {
      icon = "solar:shop-2-bold";
      color = "#99aaee";
    } else if (building.building_type == "industrial") {
      color = "#eeccaa";
    } else if (building.building_type == "other") {
      color = "#99bbaa";
    }
  }
  if (location.location_type == "landmark") {
    color = "#cc99cc";
    building = { building_type: "landmark" };
  }

  if (location.location_type == "bus stop") {
    color = "#eeaaaa";
    building = { building_type: "bus stop" };
  }
  if (location.name == "Empty") {
    color = "#444";
  }
</script>

{#if location}
  <span id={`badge-location-${location.id}`}>
    <Chip {icon} {color}>
      <span class="flex flex-row items-center gap-1">
        {#if location.name}
          <span class="text-md">{location.name}</span>
        {/if}
        {#if location.location_type == "building"}
          {#if location.name}<span>|</span>{/if}
          {street.name}
          <b
            >{building.number}
            <span>
              {#if location.apartment}
                {#if building.building_type.startsWith("residential")}
                  apt.
                {:else}
                  room
                {/if}
                {location.apartment}
              {/if}
            </span>
          </b>
        {/if}
      </span>
    </Chip>
  </span>
  <Tooltip type={"dark"} triggeredBy={`#badge-location-${location.id}`}>
    ID: {location.id} | Type: {building.building_type} | Phone: {location.phone}
  </Tooltip>
{/if}
