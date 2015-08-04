create table room_equipments (
    equip_id    integer  primary key AUTO_INCREMENT,
    equip_name  varchar(128)
);

create table room_types (
    room_type_id    integer  primary key AUTO_INCREMENT,
    room_type_name  varchar(64)
);

create table food_type (
    food_type_id        integer  primary key AUTO_INCREMENT,
    food_type_name      varchar(64)
);

create table food_category (
    food_cat_id        integer  primary key AUTO_INCREMENT,
    food_cat_name      varchar(64)
);

create table payment_status (
    ps_id   integer primary key AUTO_INCREMENT,
    ps_type varchar(64)
);

insert into payment_status values (1,'Paid');
insert into payment_status values (2,'Not Paid');
insert into payment_status values (3,'Paid Advance');

create table administrator (
    user_id     int(8) primary key AUTO_INCREMENT,
    username    varchar(128),
    password    varchar(128)

);

insert into administrator values(1,'admin','21232f297a57a5a743894a0e4a801fc3');

create table hotel_info (
    hotel_info_id   int(8) primary key AUTO_INCREMENT,
    hotel_name  varchar(256),
    address     varchar(256),
    city        varchar(128),
    dist        varchar(128),
    state       varchar(64),
    pin         varchar(32),
    phone1      varchar(32),
    phone2      varchar(32),
    email       varchar(32),
    website     varchar(128),
    contact_person      varchar(128),
    tax_id      varchar(64)
);

INSERT INTO `hotel_info` (`hotel_info_id`, `hotel_name`, `address`, `city`, `dist`, `state`, `pin`, `phone1`, `phone2`, `email`, `website`, `contact_person`, `tax_id`) VALUES ('1', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

create table customers (
    customer_id     integer primary key AUTO_INCREMENT,
    customer_name   varchar(128),
    email           varchar(64),
    address         text,
    phone           varchar(16),
    id_type         varchar(32),
    id_serial       varchar(128),
    comments        text
);


create table rooms (
    room_id     integer  primary key AUTO_INCREMENT,
    room_number varchar(16),
    room_type   varchar(32),
    room_desc   text,
    single_beds integer,
    double_beds integer,
    extra_beds  integer,
    room_equip  varchar(128),
    rate_per_room   double,
    rate_per_person double,
    room_status     integer,
    room_state      integer

);


create table customer_status (
    cs_id   integer  primary key ,
    cs_type     varchar(64)
);

insert into customer_status values (1,'Not Arrived');
insert into customer_status values (2,'Customer Checked-In');
insert into customer_status values (3,'Customer Checked-Out');


create table booking_status (
    bs_id   integer primary key AUTO_INCREMENT,
    bs_type varchar(64)
);

insert into booking_status values (1,'Booking Confirmed');
insert into booking_status values (2,'Not Confirmed');
insert into booking_status values (3,'Booking Cancled');


create table booking (
    booking_id      integer primary key AUTO_INCREMENT,
    booking_date    date,
    customer_id     integer references customers(customer_id),
    booking_from    date,
    booking_to      date,
    in_time         timestamp,
    out_time        timestamp,
    reg_serial      varchar(128),
    booking_status  integer references booking_status(bs_id),
    customer_status integer references customer_status(cs_id),
    payment_status  integer references payment_status(ps_id),
    nop             varchar(32),
    persons_name    text,
    commnets        text
);


create table room_bookings (
    room_booking_id     integer primary key AUTO_INCREMENT,
    room_rate           double,
    room_id             integer references rooms(room_id) ON DELETE CASCADE,
    booking_from        date,
    booking_to          date,
    booking_id          integer references booking(booking_id) ON DELETE CASCADE

);

create view  room_details as (SELECT room_bookings.booking_id,GROUP_CONCAT(room_number) as room_numbers FROM `room_bookings` left join rooms on room_bookings.room_id = rooms.room_id group by room_bookings.booking_id);

create view booking_details as (select booking.booking_id,booking_date,booking_from,booking_to,customers.customer_name,customers.address,customers.email,customers.phone,customers.id_type,customers.id_serial,booking.reg_serial,room_details.room_numbers,in_time,out_time,booking.nop,booking.persons_name,booking.payment_status
        from booking
                left join customers on booking.customer_id = customers.customer_id
                left join room_details on booking.booking_id = room_details.booking_id );

create view  room_booking_details as (SELECT room_bookings.booking_id,room_number,room_rate,rooms.room_type,booking_from,booking_to FROM `room_bookings` left join rooms on room_bookings.room_id = rooms.room_id );

create table identity_type (
    type_id     integer primary key AUTO_INCREMENT,
    type_name   varchar(64)

);




create table foods (
    food_id     integer  primary key AUTO_INCREMENT,
    food_name   varchar(128),
    food_type   varchar(128),
    food_category   varchar(128),
    food_price  double,
    food_desc   text
);

create table food_orders (
    order_id    integer  primary key AUTO_INCREMENT,
    food_name   varchar(128),
    order_time  timestamp,
    qty         integer,
    rate        double,
    booking_id  integer
);

create table invoice (
    inv_id              integer  primary key AUTO_INCREMENT,
    inv_number          varchar(128),
    inv_date            timestamp,
    lodging_bill_total  double,
    fooding_bill_total  double,
    room_tax            double,
    food_tax            double,
    status              integer default 0,
    booking_id          integer references booking(booking_id) ON DELETE CASCADE
);

create view invoice_details as (select booking_details.*,inv_id,inv_number,inv_date,lodging_bill_total,fooding_bill_total,room_tax,food_tax,invoice.status from invoice left join booking_details on invoice.booking_id = booking_details.booking_id);


create table expenditures(
    exp_id integer  primary key AUTO_INCREMENT,
    exp_date    timestamp,
    exp_details text,
    exp_amount  double
);


create table advance_payments(
    payment_id      integer  primary key AUTO_INCREMENT,
    payment_value   double,
    booking_id      integer references booking(booking_id) ON DELETE CASCADE
);


/*Changes*/

ALTER TABLE  `advance_payments` ADD  `payment_time` TIMESTAMP NOT NULL ;
