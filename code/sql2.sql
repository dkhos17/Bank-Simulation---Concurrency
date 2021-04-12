create table sell (
    sell_date varchar2(256) default sysdate,
    sell_item varchar2(256) constraint not null,
    sell_type varchar2(256),
    sell_num number(9),
    sell_price number(9) constraint min_price check(sell_price >= 0),
    seller_name varchar2(256),
    seller_lastname varchar2(256)
)

create table clinets(
    client_name varchar2(256),
    client_lastname varchar2(256),
    client_id number(25),
    client_number number(25),
    client_email varchar2(256),
    client_item varchar2(256)
)


insert into clients(client_name, client_lastname, client_id, client_number, client_email, clinet_item)
values ('Ithan' , 'Hant', 004, 995599434622, 'ihant17@gmail.com', 'Lamborghini Galardo');
values ('Jason' , 'Statham', 003, 995599434644, 'jstat17@gmail.com', 'Audi A8');
values ('James' , 'Bond', 007, 995599434007, 'jbond17@gmail.com', 'Aston Martin DB5');
values ('Tom' , 'Cruse', 002, 995599430002, 'tcrus17@gmail.com', 'Bugatti Veyron');
values ('Bruce' , 'Willis', 001, 995599434001, 'bwill17@gmail.com', 'Mercedes Benz G55');
values ('Bruce' , 'Lee', 010, 995599433322, 'brlee17@gmail.com', 'McLaren');
values ('Jet' , 'Lee', 011, 995599431222, 'jtlee17@gmail.com', 'Zenvo ST1');
values ('Jackie' , 'Chan', 012, 995599934622, 'jchan17@gmail.com','Lola T165');
values ('Wing' , 'Chun', 111, 995599445322, 'wchun17@gmail.com', '');
values ('Mike' , 'Tyson', 112, 995599434122, 'mtyso17@gmail.com' );
values ('John' , 'Wick', 004, 995599443322, 'jwick17@gmail.com', );
values ('Tobey' , 'Marshall', 004, 995599434622, 'tmars17@gmail.com','Ford Mustang');
values ('Ithan' , 'Hant', 004, 995599434622, 'ihant17@gmail.com' );
values ('Ithan' , 'Hant', 004, 995599434622, 'ihant17@gmail.com' );



create or replace view info 
    as (select c.client_name,
        c.client_lastname,
        s.sell_item,
        s.sell_type,
        s.sell_num,
        s.sell_price,
        s.sell_num*sell_price,
        s.sell_date,
        s.seller_name,
        s.seller_lastname
    from seller s join
        clients c on s.sell_item = c.clinet_item) 