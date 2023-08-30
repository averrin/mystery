<script>
  import Person from "./Person.svelte";
  export let persons;
</script>

<div class="flex flex-row gap-2 flex-wrap">
  {#each persons as person}
    <Person on:click person={person}/>
  {/each}
</div>
