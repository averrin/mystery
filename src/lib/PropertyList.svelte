<script>
  import Property from "./Property.svelte";
  export let person;

  let properties = {};
  for (let p of person.properties) {
    let [key, value] = p.name.split(".");
    if (value) {
      if (!properties[key]) {
        properties[key] = [];
      }
      properties[key].push({
        ...p,
        name: value.replace(/([a-z])([A-Z])/g, "$1 $2"),
      });
    } else {
      if (!properties["other"]) {
        properties["other"] = [];
      }
      properties["other"].push({
        ...p,
        name: key.replace(/([a-z])([A-Z])/g, "$1 $2"),
      });
    }
  }
  function cap(str) {
    return str.charAt(0).toUpperCase() + str.slice(1);
  }
</script>

<div class="flex flex-row gap-2 flex-wrap">
  {#each Object.entries(properties) as p}
    <div>
      <b>{cap(p[0])}:</b>
      <div class="flex flex-row gap-2 flex-wrap">
        {#each p[1] as property}
          <Property on:click {property} />
        {/each}
      </div>
    </div>
  {/each}
</div>
