<script>
  import GroupName from "./GroupName.svelte";
  import { TableBodyCell, TableBodyRow } from "flowbite-svelte";
  import PersonList from "./PersonList.svelte";
  import { Button } from "flowbite-svelte";
  import { createEventDispatcher } from "svelte";
  import Location from "./Location.svelte";
  let dispatch = createEventDispatcher();
  import cityStore from "../stores";

  export let group;

  function cap(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
  }

  function click() {
    let persons = [];
    for (let role of group.roles.map((r) => $cityStore.roles[r])) {
      for (let p of role.members.map((p) => $cityStore.persons[p])) {
        persons.push(p);
      }
    }
    dispatch("select", persons);
  }
</script>

{#if group}
  {@const roles = group.roles.map((r) => $cityStore.roles[r])}
  <TableBodyRow>
    <TableBodyCell>
      <div class="flex flex-col gap-2">
        <GroupName {group} />
        {#if group.location}
          <Location location={$cityStore.geo.locations[group.location]} />
        {/if}
      </div>
    </TableBodyCell>
    <TableBodyCell>
      <div class="flex flex-col gap-2 flex-wrap items-left">
        {#each roles as role}
          <div class="flex flex-row gap-4 flex-wrap items-center justify-start">
            <b
              >{cap(role.name)}
              {#if role.members.length > 1}({role.members.length}){/if}:</b
            >
            <PersonList
              persons={role.members.map((id) => $cityStore.persons[id])}
              on:click
            />
          </div>
        {/each}
      </div>
    </TableBodyCell><TableBodyCell>
      <Button size="xs" color="dark" on:click={click}>select</Button
      ></TableBodyCell
    >
  </TableBodyRow>
{/if}
