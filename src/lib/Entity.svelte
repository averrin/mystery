<script lang="ts">
  import { getContext } from "svelte";
  import Person from "./Person.svelte";
  import Relation from "./Relation.svelte";
  import cityStore from "../stores";

  export let id;
  let entity = $cityStore.entities[id];

  function click(e, person) {
    dispatch("click", person);
  }
</script>

{#if entity}
  <span>
    {#if entity.e_type == "relation"}
      <Relation relation={$cityStore.relations[entity.e_id]} on:click/>
    {:else if entity.e_type == "person"}
      <Person person={$cityStore.persons[entity.e_id]} on:click/>
    {/if}
  </span>
{/if}
