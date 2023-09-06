<script lang="ts">
  import { Badge } from "flowbite-svelte";
  import { createEventDispatcher } from "svelte";
  import { Tooltip, Button } from "flowbite-svelte";
  import Icon from "@iconify/svelte";
  import Chip from "./Chip.svelte";
  export let person;
  export let large = false;

  function stringToColor(str: string) {
    let hash = 0;
    str.split("").forEach((char) => {
      hash = char.charCodeAt(0) + ((hash << 5) - hash);
    });
    let colour = "#";
    for (let i = 0; i < 3; i++) {
      const value = (hash >> (i * 8)) & 0xff;
      colour += value.toString(16).padStart(2, "0");
    }
    return colour;
  }

  function formatPerson(person) {
    let s = `${person.name.first_name} `;
    if (person.name.middle_name) {
      s += `${person.name.middle_name[0]}. `;
    }
    s += `${person.name.last_name}`;
    if (person.name.maid_last_name != "") {
      s += ` [${person.name.maid_last_name}]`;
    }
    return s;
  }

  let color = person.male ? "#aaaaff" : "#eeaabb";
  let bgColor = "#232323";
  if (person.dead) {
    color = "#888";
  }

  let dispatch = createEventDispatcher();
  function click(e, person) {
    dispatch("click", person);
  }
</script>

<span
  id={`badge-person-${person.id}`}
  on:click={(e) => click(e, person)}
  class="cursor-pointer"
>
  <Chip
    icon="fluent:person-48-filled"
    {color}
    iconColor={stringToColor(person.name.last_name)}
    {bgColor}
  >
    {formatPerson(person)}
    {#if !person.local}
      <Icon icon="mingcute:car-fill" />
    {/if}
  </Chip>
</span>
<Tooltip type={"dark"} triggeredBy={`#badge-person-${person.id}`}>
  ID: {person.id}
  | Age: {person.age}
  | Wealth: {person.properties
    .find((p) => p.name == "wealth")
    ?.n.toPrecision(2)}
  | Reputation: {person.properties
    .find((p) => p.name == "reputation")
    ?.n.toPrecision(2)}
</Tooltip>
