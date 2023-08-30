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
  {@const notable = city.persons.filter(
    (p) =>
      p.tags.filter((t) => t.cat == "trait").length > 0 ||
      p.properties.find((pr) => pr.name == "wealth") > 5 ||
      p.properties.find((pr) => pr.name == "reputation") > 5 ||
      p.properties.find((pr) => pr.name == "wealth") < -2 ||
      p.properties.find((pr) => pr.name == "reputation") < -2
  )}
  {@const _notable_traits = notable
    .map((p) => p.tags.filter((t) => t.cat == "trait"))
    .flat()}
  {@const notable_traits = _notable_traits.filter(
    (t, i) => _notable_traits.find((_t) => _t.name == t.name) === t
  )}
  <div class="flex flex-wrap gap-2 items-center">
    {#each notable_traits as tag}
      <Badge
        color={{
          positive: "green",
          neutral: "yellow",
          negative: "red",
          trait: "dark",
        }[tag.cat]}
        large
        border>{tag.name}</Badge
      >
    {/each}
  </div>
  {#each notable as person (person.id)}
    <PersonDetails
      {person}
      {city}
      on:zoom={(e) => zoomPerson(e.detail)}
      on:select={(e) => selectPerson(e.detail)}
    />
  {/each}
{/if}
