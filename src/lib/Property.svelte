<script lang="ts">
  import { Badge } from "flowbite-svelte";
  import { createEventDispatcher } from "svelte";
  import { Tooltip, Button } from "flowbite-svelte";
  import Icon from "@iconify/svelte";
  import Entity from "./Entity.svelte";
  export let property;

  let color = "green";
  let entity = false;
  if (property.name == "suspect In Murder Of") {
    color = "red";
    entity = true;
  }

  if (property.name == "wealth" || property.name == "reputation") {
    color = "blue";
  }

  let dispatch = createEventDispatcher();
  function click(e, property) {
    dispatch("click", property);
  }

  function cap(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
  }
</script>

<span
  id={`badge-property-${property.name}`}
  on:click={(e) => click(e, property)}
  class="cursor-pointer"
>
  <Badge border large={true} {color}>
    <span class="flex flex-row gap-1 items-center">
      <Icon icon="mdi:tag" />
      <b>{cap(property.name)}:</b>
      {#if entity}
        <Entity id={property.n} />
      {:else if property.s != ""}
        {cap(property.s)}
      {:else}
        {property.n.toPrecision(2)}
      {/if}
    </span>
  </Badge>
</span>
