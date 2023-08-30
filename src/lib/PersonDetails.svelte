<script lang="ts">
  import Person from "./Person.svelte";
  import GroupName from "./GroupName.svelte";
  import PersonList from "./PersonList.svelte";
  import PropertyList from "./PropertyList.svelte";
  import Entity from "./Entity.svelte";
  import Icon from "@iconify/svelte";
  import Role from "./Role.svelte";
  import {
    Table,
    TableBody,
    TableBodyCell,
    TableBodyRow,
    TableHead,
    TableHeadCell,
    Button,
    Badge,
    Accordion,
    AccordionItem,
  } from "flowbite-svelte";

  import { createEventDispatcher } from "svelte";
  import Relation from "./Relation.svelte";
  let dispatch = createEventDispatcher();

  export let person;
  export let city;

  function formatDate(d) {
    return `${String(d.day).padStart(2, "0")}.${String(d.month).padStart(
      2,
      "0"
    )}.${d.year}`;
  }

  function getRelations(person) {
    return city.relations.filter(
      (r) => r.source == person.id || r.target == person.id
    );
  }

  function getPersonsSource(relType, person) {
    let rels = getRelations(person).filter((r) => r.rel_type == relType);
    let persons = [];
    for (let r of rels) {
      if (r.source != person.id && !persons.includes(r.source))
        persons.push(r.source);
    }
    return persons.map((id) => city.persons[id]);
  }

  function getPersonsTarget(relType, person) {
    let rels = getRelations(person).filter((r) => r.rel_type == relType);
    let persons = [];
    for (let r of rels) {
      if (r.target != person.id && !persons.includes(r.target))
        persons.push(r.target);
    }
    return persons.map((id) => city.persons[id]);
  }

  function getPersonsSymmetric(relType, person, active = false) {
    let rels = getRelations(person).filter((r) => r.rel_type == relType);
    if (active) {
      rels = rels.filter((r) => r.active);
    }
    let persons = [];
    for (let r of rels) {
      if (r.source != person.id && !persons.includes(r.source))
        persons.push(r.source);
      if (r.target != person.id && !persons.includes(r.target))
        persons.push(r.target);
    }
    return persons.map((id) => city.persons[id]);
  }
  function selectPerson(p) {
    dispatch("select", p);
  }

  function zoomPerson(p) {
    dispatch("zoom", p);
  }
</script>

<div class="mt-1">
  <Table striped={true}>
    <TableHead>
      <TableHeadCell colspan="2"
        ><Person {person} on:click={(e) => selectPerson(e.detail)} />
        <Button size="xs" color="dark" on:click={(_) => zoomPerson(person)}
          >zoom</Button
        >
        <Button size="xs" color="dark" on:click={(_) => selectPerson(person)}
          >select</Button
        >
      </TableHeadCell>
    </TableHead>
    <TableBody>
      <TableBodyRow>
        <TableBodyCell>Lifespan</TableBodyCell>
        <TableBodyCell>
          {formatDate(person.birth)} ({person.age}) &mdash;
          {#if person.dead}
            {formatDate(person.death)}
          {:else}
            ...
          {/if}
        </TableBodyCell>
      </TableBodyRow>
      {#if person.occupation?.title != ""}
        <TableBodyRow>
          <TableBodyCell>Occupation</TableBodyCell>
          <TableBodyCell>
            <Role role={city.roles[person.occupation.role_id]} />
          </TableBodyCell>
        </TableBodyRow>
      {/if}
      <TableBodyRow>
        <TableBodyCell>Roles</TableBodyCell>
        <TableBodyCell>
          <div class="flex flex-wrap gap-2 items-center">
            {#each city.roles.filter( (r) => r.members.includes(person.id) ) as role}
              <Role {role} />
            {/each}
          </div>
        </TableBodyCell>
      </TableBodyRow>
      <TableBodyRow>
        <TableBodyCell>Properties</TableBodyCell>
        <TableBodyCell>
          <PropertyList {person} />
        </TableBodyCell>
      </TableBodyRow>
      <TableBodyRow>
        <TableBodyCell>Tags</TableBodyCell>
        <TableBodyCell>
          <div class="flex flex-wrap gap-2 items-center">
            {#each person.tags as tag}
              <Badge
                color={{
                  positive: "green",
                  neutral: "yellow",
                  negative: "red",
                  trait: "dark",
                }[tag.cat]}
                large
                border
              >
                <span class="flex flex-row gap-1 items-center">
                  <Icon icon="mdi:tag" />
                  {tag.name}
                </span>
              </Badge>
            {/each}
          </div>
        </TableBodyCell>
      </TableBodyRow>
      <TableBodyRow>
        <TableBodyCell>Knoweledge</TableBodyCell>
        <TableBodyCell>
          <div class="flex flex-wrap gap-2 items-center">
            {#each person.knowledge as k}
              <Entity id={k} on:click={(e) => selectPerson(e.detail)} />
            {/each}
          </div>
        </TableBodyCell>
      </TableBodyRow>
    </TableBody>
  </Table>
  <Accordion>
    <AccordionItem
      defaultClass="flex items-center justify-between w-full font-medium text-left border-gray-200 dark:border-gray-700"
    >
      <span slot="header">Relations</span>
      <Table striped={true}>
        <TableBody>
          {@const spouse = getPersonsSymmetric("marriage", person, true)}
          {@const spouse_d = getPersonsSymmetric("marriage", person).filter(
            (p) => !spouse.includes(p)
          )}
          {#if spouse.length > 0 || spouse_d.length > 0}
            <TableBodyRow>
              <TableBodyCell>Spouse</TableBodyCell>
              <TableBodyCell>
                <div class="flex flex-row gap-2 flex-wrap items-center">
                  {#if spouse.length > 0}
                    <b>Married:</b>
                    <PersonList
                      persons={spouse}
                      on:click={(e) => selectPerson(e.detail)}
                    />
                  {/if}
                  {#if spouse_d.length > 0}
                    <b>Divorced:</b>
                    <PersonList
                      persons={spouse_d}
                      on:click={(e) => selectPerson(e.detail)}
                    />
                  {/if}
                </div>
              </TableBodyCell>
            </TableBodyRow>
          {/if}
          {@const children = getPersonsTarget("parent", person)}
          {#if children.length > 0}
            <TableBodyRow>
              <TableBodyCell>Children</TableBodyCell>
              <TableBodyCell>
                <PersonList
                  persons={children}
                  on:click={(e) => selectPerson(e.detail)}
                />
              </TableBodyCell>
            </TableBodyRow>
          {/if}
          {@const parents = getPersonsSource("parent", person)}
          {@const siblings = getPersonsSymmetric("sibling", person)}
          <TableBodyRow>
            <TableBodyCell>Original family</TableBodyCell>
            <TableBodyCell>
              <div class="flex flex-row gap-2 flex-wrap items-center">
                {#if parents.length > 0}
                  <b>Parents:</b>
                  <PersonList
                    persons={parents}
                    on:click={(e) => selectPerson(e.detail)}
                  />
                {/if}
                {#if siblings.length > 0}
                  <b>Siblings:</b>
                  <PersonList
                    persons={siblings}
                    on:click={(e) => selectPerson(e.detail)}
                  />
                {/if}
              </div>
            </TableBodyCell>
          </TableBodyRow>
          {@const friends = getPersonsSymmetric("friend", person)}
          {#if friends.length > 0}
            <TableBodyRow>
              <TableBodyCell>Friends</TableBodyCell>
              <TableBodyCell>
                <PersonList
                  persons={friends}
                  on:click={(e) => selectPerson(e.detail)}
                />
              </TableBodyCell>
            </TableBodyRow>
          {/if}
          {@const lovers = getPersonsSymmetric("affair", person, true)}
          {@const f_lovers = getPersonsSymmetric("affair", person).filter(
            (p) => !lovers.includes(p)
          )}
          {#if lovers.length > 0 || f_lovers.length > 0}
            <TableBodyRow>
              <TableBodyCell>Lovers</TableBodyCell>
              <TableBodyCell>
                <div class="flex flex-row gap-2 flex-wrap items-center">
                  {#if lovers.length > 0}
                    <b>Current:</b>
                    <PersonList
                      persons={lovers}
                      on:click={(e) => selectPerson(e.detail)}
                    />
                  {/if}
                  {#if f_lovers.length > 0}
                    <b>Former:</b>
                    <PersonList
                      persons={f_lovers}
                      on:click={(e) => selectPerson(e.detail)}
                    />
                  {/if}
                </div>
              </TableBodyCell>
            </TableBodyRow>
          {/if}
          {@const housemates = getPersonsSymmetric("housemate", person)}
          {#if housemates.length > 0}
            <TableBodyRow>
              <TableBodyCell>Housemates</TableBodyCell>
              <TableBodyCell>
                <PersonList
                  persons={housemates}
                  on:click={(e) => selectPerson(e.detail)}
                />
              </TableBodyCell>
            </TableBodyRow>
          {/if}
          <!-- <TableBodyRow> -->
          <!--   <TableBodyCell>All relations</TableBodyCell> -->
          <!--   <TableBodyCell> -->
          <!--     <Accordion> -->
          <!--       <AccordionItem -->
          <!--         defaultClass="flex items-center justify-between w-full font-medium text-left border-gray-200 dark:border-gray-700" -->
          <!--       > -->
          <!--         <span slot="header">All relations</span> -->
          <!--         <div class="flex flex-row gap-2 flex-wrap items-center"> -->
          <!--           {#each getRelations(person) as rel} -->
          <!--             <Relation -->
          <!--               relation={rel} -->
          <!--               on:click={(e) => selectPerson(e.detail)} -->
          <!--             /> -->
          <!--           {/each} -->
          <!--         </div> -->
          <!--       </AccordionItem> -->
          <!--     </Accordion> -->
          <!--   </TableBodyCell> -->
          <!-- </TableBodyRow> -->
        </TableBody>
      </Table>
    </AccordionItem>
  </Accordion>
</div>