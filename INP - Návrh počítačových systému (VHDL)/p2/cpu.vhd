-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2016 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): xsztef02
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (1) / zapis (0)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

   signal PC      :  std_logic_vector(11 downto 0);
   signal PTR     :  std_logic_vector(9 downto 0);
   signal TMP     :  std_logic_vector(7 downto 0);
   signal CNT     :  std_logic_vector(7 downto 0);
   signal MX_SEL  :  std_logic_vector(1 downto 0);
   signal PC_INC  :  std_logic;
   signal PC_DEC  :  std_logic;
   signal CNT_INC :  std_logic;
   signal CNT_DEC :  std_logic;
   signal PTR_INC :  std_logic;
   signal PTR_DEC :  std_logic;
   signal TMP_S   :  std_logic;
   signal CNT_RESET : std_logic;


   type States is (
      sBegin, sPinc, sPdec, sDinc, sDdec, sWhile, sWhileEnd, sWrite, sRead, sSave, sLoad,
      sEnd, sNop, sDecode , sDdecX, sDincX,sIdle,sWhileFindEnd,sWhileFindBegin,sWhileFindBegin1,sWhileFindBegin2,sWhileNext,sWhileFindEnd2
   );
   signal state : States;
   signal nextState : States; 

begin

   --hlavni proces CPU
   p_reset: process(RESET,CLK,nextState)
   begin
      if(RESET = '1') then 
        state <= sBegin;
         --nextState <= sBegin;
      elsif rising_edge(CLK) and EN = '1' then
         state <= nextState;
   end if;
   end process;
-- ----------------------------------------------------------------------------

-- ----------------------------------------------------------------------------
   p_fsm: process(RESET,CLK,OUT_BUSY,IN_VLD,DATA_RDATA,CODE_DATA,state,CNT)
   begin
      --if rising_edge(CLK) then
         -- reset hodnot
         PC_INC   <= '0';
         PC_DEC   <= '0';
         CNT_DEC  <= '0';
         CNT_INC  <= '0';
         PTR_INC  <= '0';
         PTR_DEC  <= '0';
         TMP_S    <= '0';
         DATA_EN  <= '0';
         OUT_WE   <= '0';
         IN_REQ   <= '0';
         CODE_EN  <= '1'; 
         --DATA_EN <= '1';
         DATA_RDWR <= '1';
         MX_SEL <= "00";
         CNT_RESET <= '0';
         --OUT_DATA <= (others => '0');

         case state is
            when sDecode =>
               case CODE_DATA is
                  when X"3E" => nextState <= sPinc; PC_INC <= '1';     -- >
                  when X"3C" => nextState <= sPdec; PC_INC <= '1';     -- <
                  when X"2B" => nextState <= sDinc; PC_INC <= '1';     -- +
                  when X"2D" => nextState <= sDdec;  PC_INC <= '1';    -- -
                  when X"5B" => nextState <= sWhile; PC_INC <= '1';    -- [
                  when X"5D" => nextState <= sWhileEnd;  -- ]
                  when X"2E" => nextState <= sWrite;   PC_INC <= '1';  -- .
                  when X"2C" => nextState <= sRead; PC_INC <= '1';     -- ,
                  when X"24" => nextState <= sSave; PC_INC <= '1';     -- $
                  when X"21" => nextState <= sLoad; PC_INC <= '1';     -- !
                  when X"00" => nextState <= sEnd; PC_INC <= '1';      -- null
                  when others => nextState <= sNop; PC_INC <= '1';      -- others
               end case;
               
            when sBegin =>          -- begin

               nextState <= sDecode;
            when sIdle =>
               nextState <= sDecode;
            when sPinc =>           -- >
               PTR_INC <= '1'; 
               --PC_INC <= '1';
                nextState <= sBegin;
            when sPdec =>           -- <
               PTR_DEC <= '1'; 
              --PC_INC <= '1';
                nextState <= sBegin;

            when sDincX =>           -- +
               MX_SEL <= "11"; 
               DATA_RDWR <= '0';
               DATA_EN <= '1'; 
               --PC_INC <= '1';
                nextState <= sBegin;
            when sDinc =>
               DATA_RDWR <= '1'; -- reload memread
               DATA_EN <= '1';
               nextState <= sDincX;

            when sDdecX =>           -- -
               MX_SEL <= "10"; 
               DATA_RDWR <= '0';
               DATA_EN <= '1'; 
               --PC_INC <= '1';
                nextState <= sBegin;
             when sDdec =>
               DATA_RDWR <= '1'; -- reload memread
               DATA_EN <= '1';
               nextState <= sDdecX;



            when sWhile =>          -- [
               DATA_EN <= '1';
               DATA_RDWR <= '1';
                  
               
               nextState <= sWhileNext;


            when sWhileNext =>
               if DATA_RDATA = "00000000" then      -- while(false)  preskakuju cely while
                  PC_INC <= '1';                
                  CNT_RESET <= '1';                 -- CNT = 1
                  nextState <= sWhileFindEnd;       -- while (CNT != 0) 
               else                                 -- while(true)
                  nextState <= sBegin;              -- mem neni 0 -> posouva se  
               end if;



            when sWhileFindEnd =>
               if CNT /= "00000000" then             -- while{
                  if CODE_DATA = X"5B" then            -- if (c == ’[’)
                     CNT_INC <= '1';                   -- CNT++ 
                  elsif CODE_DATA =  X"5D" then        -- elsif  elsif (c == ’]’) 
                     CNT_DEC <= '1';
                  end if;
                  nextState <= sWhileFindEnd;        -- }
                  PC_INC <= '1';
               else
                  PC_DEC <= '1';
                  nextState <= sWhileFindEnd2;               -- end
               end if; 
               --PC_INC <= '1'; 

           when sWhileFindEnd2 =>
            nextState <=sBegin;




            when sWhileEnd =>       -- ]
               DATA_EN <= '1';
               DATA_RDWR <= '1'; -- read

               if DATA_RDATA /= "00000000" then  -- vracim se na zacatek
                  PC_DEC <= '1'; 
                  CNT_RESET <= '1';  
                  nextState <= sWhileFindBegin1;                  
               else
                  nextState <= sWhileFindBegin2;   -- je 0 pokracuju dal
               end if ;

            when sWhileFindBegin1 =>
               --PC_DEC <= '1'; 
               nextState <= sWhileFindBegin;

            when sWhileFindBegin =>
                if CNT /= "00000000" then             -- while{
                  if CODE_DATA = X"5D" then            -- if (c == ’]’)
                     CNT_INC <= '1';                   -- CNT++ 
                  elsif CODE_DATA =  X"5B" then        -- elsif  elsif (c == ’[’) 
                     CNT_DEC <= '1';
                  end if;
                  nextState <= sWhileFindBegin;        -- }
                  PC_DEC <= '1';                     -- PC--
               else
                  nextState <= sWhileFindBegin2;               -- end
                  PC_INC <= '1'; 
               end if; 

            when sWhileFindBegin2 =>
               PC_INC <= '1'; 
               nextState <= sBegin;


            when sWrite =>          -- . vypisIO
                  DATA_RDWR <= '1';     -- mem read
                  DATA_EN <= '1';       --mem active
                  OUT_DATA <= DATA_RDATA;
               if OUT_BUSY = '0' then
                  
                 
                  OUT_WE <= '1';
                  --PC_INC <= '1';
                   nextState <= sBegin;
               else nextState <= sWrite;
               end if;
            when sRead =>           -- , cteniIO
               IN_REQ <= '1';
               if IN_VLD = '1' then    -- while !IN_VLD{}
                  DATA_RDWR <= '0';    -- mem write
                  DATA_EN <= '1';      -- mem active
                  MX_SEL <= "00";      -- mx - in data
                  --PC_INC <= '1';
                   nextState <= sBegin;
               else nextState <= sRead;
               end if;
            when sSave =>           -- $
               DATA_RDWR <= '1';       -- mem read
               DATA_EN <= '1';         -- mem active   
               TMP_S <= '1';           -- tmp active read
               --PC_INC <= '1';
                nextState <= sBegin;
            when sLoad =>           -- !
               DATA_RDWR <= '0';       -- mem write
               DATA_EN <= '1';         -- mem active
               MX_SEL <= "01";         -- mx - tmp 
               --PC_INC <= '1';
                nextState <= sBegin;
            when sEnd => null; nextState <= sEnd;      -- null
            when sNop => --PC_INC <= '1';
                nextState <= sBegin;
            when others => null;  nextState <= sBegin;
         end case;
     -- end if;
   end process;
-- ----------------------------------------------------------------------------
   p_pc: process(RESET,CLK,PC_DEC,PC_INC,PC)
   begin
      
      if(RESET = '1') then
         PC <= (others => '0');
      elsif rising_edge(CLK) then
         if PC_DEC ='1' then
            PC <= PC - 1;
         elsif PC_INC='1' then
            PC <= PC + 1;
         end if;
      end if;
      CODE_ADDR <= PC;
   end process;
-- ----------------------------------------------------------------------------
   p_cnt: process(CLK,RESET,CNT_DEC,CNT_INC,CNT)
   begin
      if(RESET = '1') then
         CNT <= (others => '0');
      elsif rising_edge(CLK) then
         if CNT_RESET = '1' then
            CNT <= "00000001";
         elsif CNT_INC = '1' then
            CNT <= CNT + 1;
         elsif CNT_DEC = '1' then
            CNT <= CNT - 1;
         end if;           
      end if;
   end process;
-- ----------------------------------------------------------------------------
   p_tmp: process(CLK,TMP_S)
   begin
      if rising_edge(CLK) and TMP_S = '1' then
         TMP <= DATA_RDATA;
      end if;
   end process;
-- ----------------------------------------------------------------------------
   p_ptr: process(RESET,CLK,PTR_DEC,PTR_INC,PTR)
   begin
      if(RESET = '1') then
         PTR <= (others => '0');
      elsif rising_edge(CLK) then
         if PTR_INC = '1' then
            PTR <= PTR + 1;
         elsif PTR_DEC = '1' then
            PTR <= PTR - 1;
         end if;           
      end if;
      DATA_ADDR <= PTR;
   end process;
-- ----------------------------------------------------------------------------
   p_mx: process(MX_SEL,IN_DATA,DATA_RDATA,TMP)
   begin
      CASE MX_SEL is
         when "00" => DATA_WDATA <= IN_DATA;
         when "01" => DATA_WDATA <= TMP;
         when "10" => DATA_WDATA <= DATA_RDATA - 1;
         when "11" => DATA_WDATA <= DATA_RDATA + 1;
         when others => null;
      end case;
   end process;


end behavioral;
 
