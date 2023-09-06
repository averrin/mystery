<script lang="ts">
  import { createEventDispatcher } from "svelte";
  import { Tooltip, Button } from "flowbite-svelte";
  import Person from "./Person.svelte";
  import cityStore from "../stores";

  import Chip from "./Chip.svelte";
  export let relation;

  let color = "#eee";
  let dispatch = createEventDispatcher();
  function click(e, person) {
    dispatch("click", person);
  }
</script>

<span
  id={`badge-relation-${relation.id}`}
  on:click={(e) => click(e, relation)}
  class="cursor-pointer"
>
  <Chip icon="ph:link-bold" {color}>
    <Person person={$cityStore.persons[relation.source]} on:click />
    {#if relation.symmetric}
      ⟵
    {:else}
      —
    {/if}
    {relation.rel_type}
    ⟶
    <Person person={$cityStore.persons[relation.target]} on:click />
  </Chip>
</span>
<Tooltip type={"dark"} triggeredBy={`#badge-relation-${relation.id}`}>
  ID: {relation.id}
</Tooltip>
