--http://www.adaic.org/resources/add_content/standards/05rat/html/Rat-8-2.html#I1386

generic -- this is the "template" keyword, basically
    type Element_Type is private; -- this is the value our LL will hold.
    with function "<" (Left, Right : Element_Type) return Boolean is <>; -- what's <>?
    with function "=" (Left, Right : Element_Type) return Boolean is <>;
package Ordered_List is -- the package name we expose, at global scope

    -- pragma Preelaborate(Ordered_List); -- what's this mean, copied from reference

    -- expose our main data structure type.
    type List is tagged private; -- tagged means inheritable, not sure why we use "private" here.
    -- pragma Preelaborable_Initialization(List); -- what's this mean

    type Cursor is private; -- not really used yet, but its basically an iterator.
    -- pragma Preelaborable_Initialization(Cursor);

    -- this allows us to initialize a new, empty list. Value set at bottom of this file
    Empty_List : constant List; 

    -- this is like "end()" I guess. Just initialize to null?
    No_Element : constant Cursor;

    -- define the equality function between two lists
    -- This is a function! So it treats its parameters as const.
    function "=" (Left, Right : List) return Boolean;

    -- we tag parameters in/out to mean if it should be const or not. "out" means we can write
    -- to it by reference, I think.
    procedure Insert (Container : in out List;
                      Item      : in     Element_Type);


    -- Similar to insert.
    procedure Delete (Container : in out List;
                      Item      : in     Element_Type);

    -- This is a function! So it treats its parameters as const.
    function Contains (Container : List; 
                       Item      : Element_Type) return Boolean;

    function Next (Position : Cursor) return Cursor;
    function First (Container : List) return Cursor;

    procedure Clear (Container : in out List);

private


    -- The types here are in private scope, not exposed to clients
    -- This is the usual pre-declaration of a LL node.
    type Node;
    type Node_Access is access Node; -- pointer to a node

    -- a fairly staid struct definition of a LL node.
    type Node is record
        Nxt : Node_Access;
        Data : Element_Type;
    end record;
    --with Invariant => Nxt = null or else Data <= Nxt.Data; very elegant, but apparently not allowed!

    -- so a linked list is just a head pointer.
    type List is tagged record
        Head : Node_Access;
    end record;

    -- this is basically an iterator type
    type Cursor is access Node;

    -- we explicitly define the empty_list
    Empty_List : constant List := (Head => null);

    -- we explicitly define the "no item" cursor
    No_Element : constant Cursor := null;

end Ordered_List;
