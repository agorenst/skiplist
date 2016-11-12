with Ada.Assertions; use Ada.Assertions;
package body Ordered_List is

procedure Insert (Container : in out List;
                  Item      : in     Element_Type)
is 
    N : Node_Access := Container.Head;
    P : Node_Access := null;
begin -- define code
    while N /= null and then N.data < Item loop -- "and then": short-circuiting
        P := N;
        N := N.nxt;
    end loop;
    Assert(N = null or else (Item < N.data or N.data = Item), "N is invalid");
    Assert(P = null or else P.data < Item, "P is invalid");
    Assert((P = null and N = Container.head) or else (P.nxt = N), "N/P relationship invalid");

    -- obviously we can combine these cases, but is that worth it?
    if Container.Head = null then -- empty list
        Container.Head := new Node'(Nxt => null, Data => Item);
    elsif P = null then -- element is smallest yet seen
        Container.Head := new Node'(Nxt => N, Data => Item);
    elsif N = null then -- head not null, so N got to the end
        P.nxt := new Node'(Nxt => null, Data => Item);
    else -- neither are null
        P.nxt := new Node'(Nxt => N, Data => Item);
    end if;
end Insert;


-- I'm not worrying about garbage collection yet.
procedure Delete (Container : in out List;
                  Item      : in     Element_Type)
is
    -- we define these veariables explicitly at the top.
    P : Node_Access := null; -- predecessory to n
    N : Node_Access := Container.Head; -- our iterator
begin
    while N /= null loop
        if N.data = Item then
            if P = null then -- delete the first element!
                Container.Head := N.nxt;
            else -- the usual case
                P.nxt := N.nxt;
            end if;
            exit;
        else
            -- update our state.
            P := N;
            N := N.nxt;
        end if;
    end loop;
end Delete;

-- cool, this code could probably be made neater, but hey.
function "=" (Left, Right : List) return Boolean
is
    N : Node_Access := Left.Head;
    M : Node_Access := Right.Head;
begin
    loop
        if N /= null and M /= null then
            if N.data = M.data then
                N := N.nxt;
                M := M.nxt;
            else
                return false;
            end if;
        else
            if M = N then -- both are null
                return true;
            else
                return false;
            end if;
        end if;
    end loop;
end "=";


-- similar to above.
function Contains (Container : List;
                   Item      : Element_Type) return Boolean
is
    N : Node_Access := Container.Head;
begin
    loop
        if N = null then
            return False;
        else
            if N.data = Item then
                return True;
            else
                N := N.nxt;
            end if;
        end if;
    end loop;
end Contains;

function Ordered (Container : List) return Boolean
is
    N : Node_Access := Container.Head;
begin
    if N = null then return true; end if;
    loop
        if N.nxt = null then
            return true;
        end if;
        if not (N.data < N.nxt.data or N.data = N.nxt.data) then
            return false;
        end if;
        N := N.nxt;
    end loop;
end Ordered;


function Next (Position : Cursor) return Cursor
is
begin
    return Position;
end Next;

function First (Container : List) return Cursor
is 
begin
    return Cursor'(Container.Head);
end First;

procedure Clear (Container : in out List)
is
    I : Cursor := First(Container);
begin
    null;
end Clear;

end Ordered_List;


