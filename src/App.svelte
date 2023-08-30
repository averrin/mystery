<script lang="ts">
  import { invoke } from "@tauri-apps/api/tauri";
  import {
    Toggle,
    Badge,
    Spinner,
    Button,
    Input,
    ButtonGroup,
    Checkbox,
    InputAddon,
  } from "flowbite-svelte";
  import Group from "./lib/Group.svelte";
  import Stats from "./lib/Stats.svelte";
  import Icon from "@iconify/svelte";
  import cityStore from "./stores";
  import Location from "./lib/Location.svelte";

  import {
    Table,
    TableBody,
    TableBodyCell,
    TableBodyRow,
    TableHead,
    TableHeadCell,
  } from "flowbite-svelte";
  import { Tabs, TabItem } from "flowbite-svelte";
  import { Select } from "flowbite-svelte";
  import MultiSelect from "svelte-multiselect";

  import Graph from "graphology";
  import Sigma from "sigma";
  import FA2Layout from "graphology-layout-forceatlas2/worker";
  import forceAtlas2 from "graphology-layout-forceatlas2";
  import PersonDetails from "./lib/PersonDetails.svelte";
  import NotableTab from "./lib/NotableTab.svelte";
  import MysteryTab from "./lib/MysteryTab.svelte";

  let city;
  let seed = 0;
  let customSeed = false;
  let relFlags = {
    parent: true,
    marriage: true,
    divorce: true,
    widow: true,
    sibling: false,
    friend: false,
    housemate: false,
    love: true,
    affair: true,
    coworker: false,
    rival: true,
    boss: false,
    classmate: false,
    clubmate: false,
    suspect: false,
    parishioner: false,
  };
  let show_dead = true;
  let show_linked = true;
  let show_inactive = false;

  let selectedPersons = [];
  let suggests = [];

  let modes = [
    { value: "last_name", name: "Last names" },
    { value: "gender", name: "Gender" },
    { value: "alive", name: "Alive" },
    { value: "age", name: "Age" },
    { value: "household", name: "Households" },
    { value: "orgs", name: "Orgs" },
    { value: "wealth", name: "Wealth" },
    { value: "reputation", name: "Reputation" },
  ];
  let mode = modes[0].value;

  async function init() {
    if (!customSeed) {
      seed = Math.round(Math.random() * 65000);
    } else {
      seed = Number.parseInt(seed);
    }
    console.log("generating city");
    city = await invoke("init", { seed });
    window.city = city;
    console.log(city);
    cityStore.set(city);
    suggests = city.persons.map((p) => {
      return { id: p.id, label: `${formatPerson(p)} [${p.id}]` };
    });
    initGraph();
  }
  // init(25558);
  setTimeout(init, 500);

  function formatPerson(person) {
    let s = `${person.name.first_name} `;
    if (person.name.middle_name) {
      s += `${person.name.middle_name[0]}. `;
    }
    s += `${person.name.last_name} (${person.male ? "m" : "f"})`;
    if (person.name.maid_last_name != "") {
      s += ` [${person.name.maid_last_name}]`;
    }
    return s;
  }

  function getRelations(person) {
    return city.relations.filter(
      (r) => r.source == person.id || r.target == person.id
    );
  }

  let edgeTypes = {
    parent: { type: "arrow", size: 4, color: "#4455ff", weight: 0.25 },
    marriage: { type: "line", size: 6, color: "#ff6677", weight: 0.5 },
    divorce: { type: "line", size: 4, color: "#232323", weight: 0.1 },
    widow: { type: "arrow", size: 3, color: "#666666", weight: 0.1 },
    sibling: { type: "line", size: 2, color: "#ffbb00", weight: 0 },
    friend: { type: "line", size: 1, color: "#88ff88", weight: 0.0 },
    classmate: { type: "line", size: 1, color: "#3333ff", weight: 0.0 },
    clubmate: { type: "line", size: 1, color: "#33ffff", weight: 0.0 },
    parishioner: { type: "line", size: 1, color: "#ffff33", weight: 0.0 },
    housemate: { type: "line", size: 4, color: "#88aa88", weight: 0.5 },
    love: { type: "arrow", size: 3, color: "#ff99bb", weight: 0.1 },
    affair: { type: "line", size: 4, color: "#ff99ff", weight: 0.25 },
    coworker: { type: "line", size: 4, color: "#550088", weight: 0.0 },
    rival: { type: "line", size: 1, color: "#ff3333", weight: 0.05 },
    boss: { type: "arrow", size: 4, color: "#8844aa", weight: 0.0 },
    suspect: { type: "arrow", size: 4, color: "#ff3333", weight: 0.0 },
  };

  let renderer;

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

  function isEdgeShowed(edgeType) {
    return relFlags[edgeType];
  }

  function initGraph() {
    const container = document.getElementById("sigma-container") as HTMLElement;

    const graph = new Graph({
      multi: true,
    });

    for (let p of city.persons) {
      graph.addNode(p.id, {
        x: p.id,
        y: 100 - p.age,
        size: (p.age / 50) * 10 + 5,
        label: `${formatPerson(p)} ${p.age}`,
        color: !p.dead ? stringToColor(p.name.last_name) : "#bbb",
        dead: p.dead,
      });
    }
    for (let p of city.persons) {
      for (let r of getRelations(p)) {
        if (
          city.persons.filter((p) => p.id == r.target || p.id == r.source)
            .length < 2
        )
          continue;
        graph.addEdge(r.source, r.target, {
          ...edgeTypes[r.rel_type],
          label: r.rel_type,
          active: r.active,
          rel_type: r.rel_type,
        });
      }
    }

    let layout;
    const sensibleSettings = forceAtlas2.inferSettings(graph);
    layout = new FA2Layout(graph, {
      settings: { ...sensibleSettings, weighted: true },
      weighted: true,
    });

    if (!renderer) {
      renderer = new Sigma(graph, container, {
        labelColor: { color: "#aaa" }, // YOUR SECOND FAV COLOR FOR THE LABEL
        zIndex: true,
        // hoverRenderer: customDrawHover, // IMPORTANT
        allowInvalidContainer: true,

        nodeProgramClasses: {
          // image: getNodeProgramImage(),
          // border: NodeProgramBorder,
        },
        // renderEdgeLabels: true,
      });
    } else {
      renderer.clear();
      renderer.setGraph(graph);
      // renderer.refresh();
    }
    renderer.setSetting("nodeReducer", (node, data) => {
      const res: Partial<NodeDisplayData> = { ...data };
      if (!city) return;
      let is_n = false;
      for (let n of selectedPersons.map((o) => o.id)) {
        if (graph.hasEdge(node, n) || graph.hasEdge(n, node)) {
          let _rels = city.relations
            .filter(
              (r) =>
                (r.target == node && r.source == n) ||
                (r.target == n && r.source == node)
            )
            .filter((r) => r.active);
          let rels = _rels.map((r) => isEdgeShowed(r.rel_type));
          is_n = !rels.every((i) => i === false);
          if (is_n) {
            break;
          }
        }
      }
      if (!show_linked) {
        is_n = false;
      }

      let p = city.persons[node];
      if (mode == "last_name") {
        res.color = !data.dead ? stringToColor(p.name.last_name) : "#bbb";
      } else if (mode == "household") {
        let roles = city.roles.filter((r) => r.members.includes(p.id));
        if (roles.length == 0) {
          res.color = "#bbb";
        } else {
          let hh = city.groups
            .filter((g) => g.group_type == "household")
            .find((g) => roles.map((r) => r.group_id).includes(g.id));
          if (!hh) {
            res.color = "#bbb";
          } else {
            let name = `${hh.id} ${hh.name}`;
            res.color = !data.dead ? stringToColor(name) : "#bbb";
          }
        }
      } else if (mode == "orgs") {
        let roles = city.roles.filter((r) => r.members.includes(p.id));
        if (roles.length == 0) {
          res.color = "#bbb";
          res.hidden = true;
        } else {
          let hh = city.groups
            .filter((g) => g.group_type != "household")
            .find((g) => roles.map((r) => r.group_id).includes(g.id));
          if (!hh) {
            res.color = "#bbb";
            res.hidden = true;
          } else {
            let name = `${hh.id} ${hh.name}`;
            res.color = !data.dead ? stringToColor(name) : "#bbb";
          }
        }
      } else if (mode == "gender") {
        res.color = p.male ? "#4444ff" : "#ff88aa";
      } else if (mode == "alive") {
        res.color = p.dead ? "#232323" : "#23aa23";
      } else if (mode == "age") {
        if (p.age < 18) {
          res.color = "#88ffbb";
        } else if (p.age < 35) {
          res.color = "#22ff88";
        } else if (p.age < 60) {
          res.color = "#22cc88";
        } else {
          res.color = "#228855";
        }
        if (p.dead) {
          res.hidden = true;
        }
      } else if (mode == "wealth") {
        let prop = p.properties.find((pr) => pr.name == "wealth");
        res.color = "#232323";
        if (prop !== undefined) {
          if (prop.n < 0) {
            res.color = "#ff8888";
          } else if (prop.n < 2) {
            res.color = "#ffbbbb";
          } else if (prop.n < 3) {
            res.color = "#eeffee";
          } else if (prop.n < 4) {
            res.color = "#ccffcc";
          } else if (prop.n < 5) {
            res.color = "#aaffaa";
          } else if (prop.n < 6) {
            res.color = "#88ff88";
          } else if (prop.n >= 6) {
            res.color = "#55ff55";
          }
        }
        if (p.dead || p.age < 18) {
          res.hidden = true;
        }
      } else if (mode == "reputation") {
        let prop = p.properties.find((pr) => pr.name == "reputation");
        res.color = "#ccc";
        if (prop !== undefined) {
          if (prop.n < 0) {
            res.color = "#ff8888";
          } else if (prop.n < 1) {
            res.color = "#eeeeff";
          } else if (prop.n < 2) {
            res.color = "#eeffee";
          } else if (prop.n < 3) {
            res.color = "#ccffcc";
          } else if (prop.n < 4) {
            res.color = "#aaffaa";
          } else if (prop.n < 5) {
            res.color = "#88ff88";
          } else if (prop.n >= 5) {
            res.color = "#55ff55";
          }
        }
        if (p.dead || p.age < 18) {
          res.hidden = true;
        }
      }

      if (
        (!show_dead && data.dead) ||
        (selectedPersons.length > 0 &&
          !selectedPersons.map((o) => o.id).includes(Number.parseInt(node)) &&
          !is_n)
      ) {
        res.hidden = true;
      }

      if (is_n && !res.hidden) {
        // res.type = "border";
      }

      // if (searchQ.length > 0 && res.label.includes(searchQ)) {
      //   res.highlighted = true;
      // }

      return res;
    });

    renderer.setSetting("edgeReducer", (edge, data) => {
      const res: Partial<EdgeDisplayData> = { ...data };
      if (!city) return;
      let linked = false;
      for (let p of selectedPersons.map((o) => o.id)) {
        if (graph.hasExtremity(edge, p)) {
          linked = true;
          break;
        }
      }
      if (!show_dead) {
        for (let p of city.persons) {
          if (!p.dead) continue;
          if (graph.hasExtremity(edge, p.id)) {
            res.hidden = true;
            return res;
          }
        }
      }

      let show = isEdgeShowed(res.rel_type);
      let is_inactive = !show_inactive && !data.active;
      if (is_inactive || (selectedPersons.length > 0 && !linked) || !show) {
        res.hidden = true;
      } else {
        res.hidden = false;
      }

      return res;
    });

    let draggedNode: string | null = null;
    let isDragging = false;

    renderer.on("downNode", (e) => {
      isDragging = true;
      draggedNode = e.node;
      graph.setNodeAttribute(draggedNode, "highlighted", true);
      layout.stop();
      let person = city.persons.find(
        (p) => p.id == Number.parseInt(draggedNode)
      );
      if (!selectedPersons.find((p) => p.id == person.id)) {
        selectedPersons = [
          ...selectedPersons,
          { id: person.id, label: formatPerson(person) },
        ];
      } else {
        selectedPersons = selectedPersons.filter((p) => p.id != person.id);
      }
    });

    renderer.getMouseCaptor().on("mousemovebody", (e) => {
      if (!isDragging || !draggedNode) return;

      // Get new position of node
      const pos = renderer.viewportToGraph(e);

      graph.setNodeAttribute(draggedNode, "x", pos.x);
      graph.setNodeAttribute(draggedNode, "y", pos.y);

      // Prevent sigma to move camera:
      e.preventSigmaDefault();
      e.original.preventDefault();
      e.original.stopPropagation();
    });

    renderer.getMouseCaptor().on("mouseup", () => {
      if (draggedNode) {
        graph.removeNodeAttribute(draggedNode, "highlighted");
      }
      isDragging = false;
      draggedNode = null;
    });

    renderer.getMouseCaptor().on("mousedown", () => {
      if (!renderer.getCustomBBox()) renderer.setCustomBBox(renderer.getBBox());
    });

    layout.start();
  }

  function selectPerson(p) {
    if (!selectedPersons.find((_p) => _p.id == p.id)) {
      selectedPersons = [
        ...selectedPersons,
        { id: p.id, label: formatPerson(p) },
      ];
    }
    zoomPerson(p);
  }

  function zoomPerson(p) {
    const nodePosition = renderer.getNodeDisplayData(p.id);
    renderer.getCamera().animate(nodePosition, {
      duration: 500,
    });
    setTimeout(() => {
      renderer.getCamera().animatedZoom(2);
    }, 550);
  }

  function getSelectedGroups() {
    let groups = [];
    for (let p of selectedPersons) {
      for (let role of city.roles.filter((r) => r.members.includes(p.id))) {
        groups.push(city.groups[role.group_id]);
      }
    }
    return groups;
  }

  let selectedFlags = Object.entries(relFlags)
    .filter((p) => p[1])
    .map((p) => {
      return { id: p[0], label: p[0] };
    });

  function update() {
    console.log(relFlags);
    renderer.refresh();
    // initGraph();
  }
</script>

<div id="sigma-container">
  <div class="dark m-4 flex flex-row gap-4 flex-wrap items-center absolute">
    {#if !city}
      <div class="text-center w-full h-full">
        <Spinner color="blue" size={10} />
      </div>
    {:else}
      <ButtonGroup>
        <InputAddon>
          <Checkbox bind:checked={customSeed} />
        </InputAddon>
        <Input class="md-6 w-24" disabled={!customSeed} bind:value={seed} />
        <Button
          color="blue"
          on:click={(_) => {
            city = undefined;
            setTimeout(init, 200);
          }}
        >
          <Icon icon="charm:refresh" />
        </Button>
      </ButtonGroup>
      <span class="font-bold">Coloring</span>
      <Select class="w-48" items={modes} bind:value={mode} />
    {/if}
  </div>
</div>
<main class="container dark flex flex-col gap-2 m-8 w-full">
  {#if city}
    <div class="flex flex-row gap-4 flex-wrap items-center">
      <div class="w-1/2">
        <MultiSelect
          inputClass="md-6"
          outerDivClass="!border-gray-500"
          liSelectedClass="border bg-gray-100 border-gray-500 flex flex-row items-center"
          ulOptionsClass={"!bg-gray-900"}
          options={Object.entries(relFlags).map((p) => {
            return { id: p[0], label: p[0] };
          })}
          bind:selected={selectedFlags}
          on:change={(_) => {
            Object.entries(relFlags).forEach((p) => (relFlags[p[0]] = false));
            console.log(selectedFlags);
            selectedFlags.forEach((e) => (relFlags[e.id] = true));
            update();
          }}
        />
      </div>
      <div class="flex flex-row gap-4 flex-wrap">
        <Toggle bind:checked={show_dead} on:change={update}>Dead</Toggle>
        <Toggle bind:checked={show_linked} on:change={update}>Linked</Toggle>
        <Toggle bind:checked={show_inactive} on:change={update}
          >Inactive rels</Toggle
        >
      </div>
    </div>

    <div class="flex flex-row gap-4 flex-wrap items-center">
      <div class="w-5/6">
        <MultiSelect
          inputClass="md-6"
          outerDivClass="!border-gray-500"
          liSelectedClass="border bg-gray-100 border-gray-500 flex flex-row items-center"
          ulOptionsClass={"!bg-gray-900"}
          options={suggests}
          bind:selected={selectedPersons}
          on:change={(_) => console.log(selectedPersons)}
        />
      </div>
      <Button
        on:click={(_) => {
          selectedPersons = [];
          update();
        }}>Clear</Button
      >
    </div>

    <Tabs>
      <TabItem open title="Map">
        {#each city.geo.streets.filter(s => s.street_type) as street}
          <div>
            <span class="font-bold text-xl">{street.name} [{street.street_type}]</span>
            <div class="mr-6 flex flex-col gap-1">
              {#each city.geo.buildings.filter((b) => b.street == street.id) as building}
                  <div class="mr-6 flex flex-row gap-2 flex-wrap items-center">
                    {#each city.geo.locations.filter((l) => l.building == building.id) as location}
                        <Location {location} />
                    {/each}
                  </div>
              {/each}
            </div>
          </div>
        {/each}
        {#each city.geo.streets.filter(s => s.street_type) as street}
          <div>
            <span class="font-bold text-xl">{street.name} [EMPTY]</span>
          </div>
        {/each}
      </TabItem>
      <TabItem title="Stats">
        <Stats {city} />
      </TabItem>
      <TabItem title="Mystery">
        <MysteryTab
          {city}
          on:zoom={(e) => zoomPerson(e.detail)}
          on:select={(e) => selectPerson(e.detail)}
        />
      </TabItem>
      <TabItem title="Notable">
        <NotableTab
          {city}
          on:zoom={(e) => zoomPerson(e.detail)}
          on:select={(e) => selectPerson(e.detail)}
        />
      </TabItem>
      <TabItem title="Orgs">
        <Table striped={true}>
          <TableHead />
          <TableBody>
            {@const groups =
              selectedPersons.length > 0 ? getSelectedGroups() : city.groups}
            {#each groups?.filter((g) => g?.group_type != "household") as group (group?.id)}
              <Group
                {group}
                on:click={(e) => selectPerson(e.detail)}
                on:select={(e) => e.detail.forEach((p) => selectPerson(p))}
              />
            {/each}
          </TableBody>
        </Table>
      </TabItem>
      <TabItem title="Households">
        <Table striped={true}>
          <TableHead />
          <TableBody>
            {@const hh =
              selectedPersons.length > 0 ? getSelectedGroups() : city.groups}
            {#each hh.filter((g) => g.group_type == "household") as group (group.id)}
              <Group
                {group}
                on:click={(e) => selectPerson(e.detail)}
                on:select={(e) => e.detail.forEach((p) => selectPerson(p))}
              />
            {/each}
          </TableBody>
        </Table>
      </TabItem>
      <TabItem title="Persons">
        {@const persons =
          selectedPersons.length > 0
            ? selectedPersons.map((i) => city.persons[i.id])
            : city.persons}
        {#each persons as person (person.id)}
          <PersonDetails
            {person}
            {city}
            on:zoom={(e) => zoomPerson(e.detail)}
            on:select={(e) => selectPerson(e.detail)}
          />
        {/each}
      </TabItem>
    </Tabs>
  {/if}
</main>

<style>
  #sigma-container {
    width: 100%;
    height: 800px;
    margin: 0;
    padding: 0;
    overflow: hidden;
    background-color: #222;
  }
  :global(div.multiselect > ul.options) {
    background-color: #212121;
  }
</style>
