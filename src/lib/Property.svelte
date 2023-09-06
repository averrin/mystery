<script lang="ts">
  import { createEventDispatcher } from "svelte";
  import Chip from "./Chip.svelte";
  import Entity from "./Entity.svelte";
  export let property;

  // let color = "#99bbaa";
  let color = "#999";
  let entity = false;
  if (property.name == "suspect In Murder Of") {
    color = "#eeaaaa";
    entity = true;
  }

  if (property.name == "wealth" || property.name == "reputation") {
    color = "#99aaee";
  }

  let dispatch = createEventDispatcher();
  function click(e, property) {
    dispatch("click", property);
  }

  function cap(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
  }
</script>

<Chip icon="mdi-tag" {color}>
  <b>{cap(property.name)}:</b>
  {#if entity}
    <Entity id={property.n} />
  {:else if property.s != ""}
    {cap(property.s)}
  {:else}
    {property.n.toPrecision(2)}
  {/if}
</Chip>
