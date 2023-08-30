<script lang="ts">
  import { Badge } from "flowbite-svelte";
  import PersonDetails from "./PersonDetails.svelte";
  export let city;

  import { createEventDispatcher } from "svelte";
  let dispatch = createEventDispatcher();

  function selectPerson(p) {
    dispatch("select", p);
  }

  function zoomPerson(p) {
    dispatch("zoom", p);
  }
</script>

{#if city}
  {@const notable = city.persons.filter((p) =>
    p.tags.find((t) => t.name == "Murdered")
  )}
  <div class="p-4 font-bold text-lg">Murdered:</div>
  {#each notable as person (person.id)}
    <PersonDetails
      {person}
      {city}
      on:zoom={(e) => zoomPerson(e.detail)}
      on:select={(e) => selectPerson(e.detail)}
    />
  {/each}
  {@const suspects = city.persons.filter((p) =>
    p.properties.find((t) => t.name == "suspectInMurderOf")
  )}
  <div class="p-4 font-bold text-lg">
    Suspects ({suspects.length}):
  </div>
  {#each suspects as person (person.id)}
    <PersonDetails
      {person}
      {city}
      on:zoom={(e) => zoomPerson(e.detail)}
      on:select={(e) => selectPerson(e.detail)}
    />
  {/each}
{/if}
