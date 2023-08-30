<script>
  export let city;

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

  function countLoveSymmetric() {
    let c = 0;
    let loves = city.relations.filter((r) => r.rel_type == "love");
    for (let r of loves) {
      for (let r2 of loves) {
        if (r.source == r2.target && r.target == r2.source) {
          c++;
          break;
        }
      }
    }
    return c / 2;
  }

  function countLoveMarriage() {
    let c = 0;
    let loves = city.relations.filter((r) => r.rel_type == "love");
    for (let r of loves) {
      for (let r2 of loves) {
        if (r.source == r2.target && r.target == r2.source) {
          let m = getPersonsSymmetric("marriage", city.persons[r.source]);
          if (m.length > 0 && (m[0].id == r.target || m[0].id == r.source)) {
            c++;
          }
          break;
        }
      }
    }
    return c / 2;
  }

  import Chart from "./Chart.svelte";
</script>

<div class="flex flex-row gap-4 flex-wrap items-center">
  <Chart
    data={[
      { value: city.persons.filter((p) => p.male).length, color: "#8888FF", label: "Male" },
      { value: city.persons.filter((p) => !p.male).length, color: "#FF88aa" , label: "Female"},
    ]}
  />
  <Chart
    data={[
      { value: city.persons.filter((p) => !p.dead).length, color: "#88cc88", label: "Alive" },
      { value: city.persons.filter((p) => p.dead).length, color: "#888" , label: "Dead"},
    ]}
  />

  <Chart
    data={[
      { value: city.persons.filter((p) => p.age < 7).length, color: "#cceecc", label: "< 7" },
      { value: city.persons.filter((p) => p.age < 18 && p.age >= 7).length, color: "#5555ff", label: "7 - 18" },
      { value: city.persons.filter((p) => p.age >= 18 && p.age < 65).length, color: "#55ee55", label: "18 - 65" },
      { value: city.persons.filter((p) => p.age >= 65).length, color: "#338833", label: ">= 65" },
    ]}
  />

  <Chart
    data={[
      { value: city.persons.filter((p) => p.local).length, color: "#338833", label: "Local" },
      { value: city.persons.filter((p) => !p.local).length, color: "#883333" , label: "External"},
    ]}
  />

  <span
    ><b>With traits</b>: {city.persons.filter(
      (p) => p.tags.filter((t) => t.cat == "trait").length > 0
    ).length}</span
  >

  <span
    ><b>Discarded</b>: {city.persons.filter(
      (p) =>
        getPersonsSymmetric("parent", p).length == 0 &&
        getPersonsSymmetric("marriage", p).length == 0
    ).length}</span
  >
  <span
    ><b>Single</b>: {city.persons.filter(
      (p) => p.age >= 18 && getPersonsSymmetric("marriage", p).length == 0
    ).length}</span
  >
  <span
    ><b>Lone</b>: {city.persons.filter(
      (p) =>
        p.age >= 18 &&
        !p.dead &&
        getPersonsSymmetric("marriage", p).length == 0 &&
        getPersonsSymmetric("affair", p).length == 0
    ).length}</span
  >
  <span
    ><b>Widowed</b>: {city.persons.filter(
      (p) => getPersonsSymmetric("widow", p).length > 0
    ).length}</span
  >
  <span
    ><b>Affairs</b>: {city.persons.filter(
      (p) => getPersonsSymmetric("affair", p).length > 0
    ).length}</span
  >
  <span
    ><b>Children</b>: &nbsp;0: {city.persons.filter(
      (p) => p.age >= 18 && getPersonsTarget("parent", p).length == 0
    ).length -
      city.persons.filter(
        (p) => p.age >= 18 && getPersonsSymmetric("marriage", p).length == 0
      ).length /
        2}
    &nbsp;1: {city.persons.filter(
      (p) => p.age >= 18 && getPersonsTarget("parent", p).length == 1
    ).length / 2}
    &nbsp;2: {city.persons.filter(
      (p) => p.age >= 18 && getPersonsTarget("parent", p).length == 2
    ).length / 2}
    &nbsp;3: {city.persons.filter(
      (p) => p.age >= 18 && getPersonsTarget("parent", p).length == 3
    ).length / 2}
    &nbsp;4: {city.persons.filter(
      (p) => p.age >= 18 && getPersonsTarget("parent", p).length == 4
    ).length / 2}
  </span>
  <span
    ><b>Friendships</b>: {city.relations.filter((p) => p.rel_type == "friend")
      .length}</span
  >
  <span
    ><b>Marriages</b>: {city.relations.filter(
      (p) => p.rel_type == "marriage" && p.active
    ).length}/{city.relations.filter(
      (p) => p.rel_type == "marriage" && !p.active
    ).length}/{city.persons.filter(
      (p) => getPersonsSymmetric("divorce", p).length > 0
    ).length}
  </span>
  <span
    ><b>Loves</b>: {city.relations.filter((p) => p.rel_type == "love")
      .length}/{countLoveSymmetric()}/{countLoveMarriage()}</span
  >
  <span
    ><b>Households</b>: {city.groups.filter((g) => g.group_type == "household")
      .length}/{city.groups.filter(
      (g) => city.roles[g.roles[0]]?.members?.length == 1
    ).length}</span
  >
  <span
    ><b>Orgs</b>: {city.groups.filter((g) => g.group_type != "household")
      .length}</span
  >
  <span
    ><b>Employed</b>: {city.persons.filter((p) => p.occupation?.title != "")
      .length}
    ({Math.round(
      (city.persons.filter((p) => p.occupation?.title != "").length /
        city.persons.filter((p) => !p.dead && p.age >= 18 && p.age <= 65)
          .length) *
        100
    )}%)</span
  >
  <span
    ><b>Unemployed</b>: {city.persons.filter(
      (p) => !p.dead && p.age >= 18 && p.age <= 65 && p.occupation?.title == ""
    ).length}
    ({city.persons.filter(
      (p) =>
        !p.dead &&
        p.male &&
        p.age >= 18 &&
        p.age <= 65 &&
        p.occupation?.title == ""
    ).length}/{city.persons.filter(
      (p) =>
        !p.dead &&
        !p.male &&
        p.age >= 18 &&
        p.age <= 65 &&
        p.occupation?.title == ""
    ).length})
  </span>
</div>
