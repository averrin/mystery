pub use ffi::*;

#[cxx::bridge]
mod ffi {
    #[derive(Serialize, Debug)]
    struct Entity {
        id: i32,
        e_type: String,
        e_id: i32,
    }
    #[derive(Serialize, Debug)]
    struct Tag {
        name: String,
        cat: String,
    }
    #[derive(Serialize, Debug)]
    struct Property {
        name: String,
        cat: String,
        n: f32,
        s: String,
    }
    #[derive(Serialize, Debug)]
    struct Occupation {
        title: String,
        role_id: i32,
        group_id: i32,
    }
    #[derive(Serialize, Debug)]
    struct GroupRole {
        id: i32,
        group_id: i32,
        name: String,
        members: Vec<i32>,
    }
    #[derive(Serialize, Debug)]
    struct Group {
        id: i32,
        name: String,
        group_type: String,
        roles: Vec<i32>,
        properties: Vec<i32>,
        location: i32,
    }
    #[derive(Serialize, Debug)]
    struct Dates {
        id: i32,
        day: i32,
        month: i32,
        year: i32,
    }
    #[derive(Serialize, Debug)]
    struct Names {
        id: i32,
        first_name: String,
        middle_name: String,
        last_name: String,
        maid_last_name: String,
    }

    #[derive(Serialize, Debug)]
    struct Person {
        id: i32,
        name: Names,
        alias: Names,
        birth: Dates,
        death: Dates,
        dead: bool,
        age: i32,
        male: bool,
        local: bool,
        occupation: Occupation,
        properties: Vec<Property>,
        tags: Vec<Tag>,
        knowledge: Vec<i32>,
    }

    #[derive(Serialize, Debug)]
    struct Relation {
        id: i32,
        source: i32,
        target: i32,
        rel_type: String,
        rel_cat: String,
        value: f32,
        symmetric: bool,
        active: bool,
    }

    #[derive(Serialize, Debug)]
    struct Phone {
        id: i32,
        number: String,
        persons: Vec<i32>,
    }
    #[derive(Serialize, Debug)]
    struct Street {
        id: i32,
        name: String,
        street_type: String,
    }
    #[derive(Serialize, Debug)]
    struct Building {
        id: i32,
        building_type: String,
        street: i32,
        number: i32,
    }
    #[derive(Serialize, Debug)]
    struct Location {
        id: i32,
        name: String,
        properties: Vec<i32>,
        tags: Vec<i32>,
        phone: i32,
        building: i32,
        apartment: i32,
    }
    #[derive(Serialize, Debug)]
    struct Event {
        id: i32,
        name: String,
        date: Dates,
        location: i32,
        persons: Vec<i32>,
        properties: Vec<i32>,
        tags: Vec<i32>,
    }
    #[derive(Serialize, Debug)]
    struct Mystery {
        murdered: i32,
        killer: i32,
        motive: i32,
        suspects: Vec<i32>,
        motives: Vec<i32>,
        clues: Vec<i32>,
    }
    #[derive(Serialize, Debug)]
    struct Geo {
        locations: Vec<Location>,
        phones: Vec<Phone>,
        buildings: Vec<Building>,
        streets: Vec<Street>,
    }
    #[derive(Serialize, Debug)]
    struct City {
        seed: i32,
        name: String,
        persons: Vec<Person>,
        relations: Vec<Relation>,
        groups: Vec<Group>,
        roles: Vec<GroupRole>,
        entities: Vec<Entity>,
        mystery: Mystery,
        events: Vec<Event>,
        geo: Geo,
    }
}
