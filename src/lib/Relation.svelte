<script lang="ts">
  import { Badge } from "flowbite-svelte";
  import { createEventDispatcher } from "svelte";
  import { Tooltip, Button } from "flowbite-svelte";
  import Icon from "@iconify/svelte";
  import Person from "./Person.svelte";
  import cityStore from "../stores";
  export let relation;

  let color = "dark";
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
  <Badge border large={true} {color}>
    <span class="flex flex-row gap-2 items-center">
      <Icon icon="ph:link-bold" />
      <Person person={$cityStore.persons[relation.source]} on:click/>
      {#if relation.symmetric}
        ⟵
      {:else}
        —
      {/if}
      {relation.rel_type}
      ⟶
      <Person person={$cityStore.persons[relation.target]} on:click/>
    </span>
  </Badge>
</span>
<Tooltip type={"dark"} triggeredBy={`#badge-relation-${relation.id}`} >
  ID: {relation.id}
</Tooltip>
