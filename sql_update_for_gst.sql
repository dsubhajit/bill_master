
# Rooms Table Update

ALTER TABLE `rooms` ADD `cgst` DOUBLE NULL DEFAULT '0' AFTER `room_state`, ADD `sgst` DOUBLE NULL DEFAULT '0' AFTER `cgst`, ADD `gst` DOUBLE NULL DEFAULT '0' AFTER `sgst`;



# Room Booking Details 

ALTER TABLE `room_bookings` ADD `cgst` DOUBLE NULL DEFAULT '0' AFTER `room_id`, ADD `sgst` DOUBLE NULL DEFAULT '0' AFTER `cgst`;


create view  room_booking_details as (SELECT room_bookings.booking_id,room_number,room_bookings.room_rate,rooms.room_type,booking_from,booking_to,room_bookings.cgst,room_bookings.sgst FROM `room_bookings` left join rooms on room_bookings.room_id = rooms.room_id );


ALTER TABLE `hotel_info` CHANGE `tax_id` `gst_number` VARCHAR(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NULL DEFAULT NULL;


ALTER TABLE `hotel_info` ADD `food_cgst` DOUBLE NULL DEFAULT '0' AFTER `gst_number`, ADD `food_sgst` DOUBLE NULL DEFAULT '0' AFTER `food_cgst`;


ALTER TABLE `foods` ADD `hsn_code` VARCHAR(32) NULL DEFAULT NULL AFTER `food_desc`, ADD `sgst` DOUBLE NULL AFTER `hsn_code`, ADD `cgst` DOUBLE NULL AFTER `sgst`;


ALTER TABLE `food_orders` ADD `hsn_code` VARCHAR(64) NULL AFTER `rate`, ADD `sgst` DOUBLE NOT NULL DEFAULT '0' AFTER `hsn_code`, ADD `cgst` DOUBLE NOT NULL DEFAULT '0' AFTER `sgst`;


CREATE VIEW food_order_detailsfull as ( select order_id,hsn_code,food_name,order_time,qty,rate, sgst,cgst, ( (qty*rate*(cgst+sgst))/100 ) as tax_amount,(( (qty*rate*(cgst+sgst))/100 ) +(qty*rate)) as total_amount,booking_id from food_orders);


CREATE TABLE gst_data (
	data_id 	integer		AUTO_INCREMENT primary key,
	cgst_amount	double,
	sgst_amount	double,
	booking_id  integer	references booking(booking_id),
	inv_id		integer	references	invoice(inv_id),
	inv_number	varchar(64),
	inv_date	date		
);

ALTER TABLE `invoice` CHANGE `inv_date` `inv_date` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP;

SELECT  m, Count(gst_data.data_id) as count FROM (SELECT y, m FROM (SELECT 2017 y ) years, (SELECT 1 m UNION ALL SELECT 2 UNION ALL SELECT 3 UNION ALL SELECT 4 UNION ALL SELECT 5 UNION ALL SELECT 6 UNION ALL SELECT 7 UNION ALL SELECT 8 UNION ALL SELECT 9 UNION ALL SELECT 10 UNION ALL SELECT 11 UNION ALL SELECT 12) months ) ym  LEFT JOIN gst_data on ym.y = YEAR(gst_data.inv_date) and ym.m = MONTH(gst_data.inv_date)  group by m;


SELECT  m, SUM(gst_data.cgst_amount) as cgst_amount, SUM(gst_data.sgst_amount) as sgst_amount FROM (SELECT y, m FROM (SELECT 2017 y ) years, (SELECT 1 m UNION ALL SELECT 2 UNION ALL SELECT 3 UNION ALL SELECT 4 UNION ALL SELECT 5 UNION ALL SELECT 6 UNION ALL SELECT 7 UNION ALL SELECT 8 UNION ALL SELECT 9 UNION ALL SELECT 10 UNION ALL SELECT 11 UNION ALL SELECT 12) months ) ym  LEFT JOIN gst_data on ym.y = YEAR(gst_data.inv_date) and ym.m = MONTH(gst_data.inv_date)  group by m




ALTER TABLE `customers` ADD `gstin_no` VARCHAR(20) NULL DEFAULT NULL AFTER `comments`;


create view booking_details as (select booking.booking_id,booking_date,booking_from,booking_to,customers.customer_name,customers.address,customers.email,customers.phone,customers.id_type,customers.id_serial,customers.gstin_no,booking.reg_serial,room_details.room_numbers,in_time,out_time,booking.nop,booking.persons_name,booking.payment_status
        from booking
                left join customers on booking.customer_id = customers.customer_id
                left join room_details on booking.booking_id = room_details.booking_id );


ALTER TABLE `invoice` ADD `inv_sl` INT NOT NULL AFTER `inv_id`;



