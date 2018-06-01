-- fsm.vhd: Finite State Machine
-- Author(s): 
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (TEST1, PRINT_MESSAGE, FINISH,
	START,FINISH_ERR,FINISH_OK,ERROR,KEY1_2,KEY2_8,
	KEY3_6,KEY4_8,KEY5_2,KEY6_3,KEY7_2,KEY8_2,KEY9_5,KEY10_2,
	KEY3_3,KEY4_4,KEY5_1,KEY6_1,KEY7_6,KEY8_1,KEY9_2,KEY10_6,KEY11_4);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= START;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when START =>
      next_state <= START;
      if (KEY(15) = '1') then
         next_state <= FINISH_ERR;
		elsif (KEY(2) = '1') then
			next_state <= KEY1_2;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when KEY1_2 =>
      next_state <= KEY1_2;
      if (KEY(15) = '1') then
         next_state <= FINISH_ERR;
		elsif (KEY(8) = '1') then
			next_state <= KEY2_8;	
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when KEY2_8 =>
      next_state <= KEY2_8;
      if (KEY(15) = '1') then
         next_state <= FINISH_ERR;
		elsif (KEY(6) = '1') then
			next_state <= KEY3_6;
		elsif (KEY(3) = '1') then
			next_state <= KEY3_3;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when KEY3_6 =>
      next_state <= KEY3_6;
      if (KEY(15) = '1') then
         next_state <= FINISH_ERR;
		elsif (KEY(8) = '1') then
			next_state <= KEY4_8;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;
-- - - - - - - - - - - - - - - - - - - - - - -
   when KEY4_8 =>
      next_state <= KEY4_8;
      if (KEY(15) = '1') then
         next_state <= FINISH_ERR;
		elsif (KEY(2) = '1') then
			next_state <= KEY5_2;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;
-- - - - - - - - - - - - - - - - - - - - - - -
   when KEY5_2 =>
      next_state <= KEY5_2;
      if (KEY(15) = '1') then
         next_state <= FINISH_ERR;
		elsif (KEY(3) = '1') then
			next_state <= KEY6_3;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;
-- - - - - - - - - - - - - - - - - - - - - - -
   when KEY6_3 =>
      next_state <= KEY6_3;
      if (KEY(15) = '1') then
         next_state <= FINISH_ERR;
		elsif (KEY(2) = '1') then
			next_state <= KEY7_2;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;
-- - - - - - - - - - - - - - - - - - - - - - -
   when KEY7_2 =>
      next_state <= KEY7_2;
      if (KEY(15) = '1') then
         next_state <= FINISH_ERR;
		elsif (KEY(2) = '1') then
			next_state <= KEY8_2;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;
-- - - - - - - - - - - - - - - - - - - - - - -
   when KEY8_2 =>
      next_state <= KEY8_2;
      if (KEY(15) = '1') then
         next_state <= FINISH_ERR;
		elsif (KEY(5) = '1') then
			next_state <= KEY9_5;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;
-- - - - - - - - - - - - - - - - - - - - - - -
   when KEY9_5 =>
      next_state <= KEY9_5;
      if (KEY(15) = '1') then
         next_state <= FINISH_ERR;
		elsif (KEY(2) = '1') then
			next_state <= KEY10_2;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;
-- - - - - - - - - - - - - - - - - - - - - - -
   when KEY10_2 =>
      next_state <= KEY10_2;
      if (KEY(15) = '1') then
         next_state <= FINISH_OK;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;
-- - - - - - - - - - - - - - - - - - - - - - -
-- - - - - - - - - - - - - - - - - - - - - - -
   when KEY3_3 =>
      next_state <= KEY3_3;
      if (KEY(15) = '1') then
         next_state <= FINISH_ERR;
		elsif (KEY(4) = '1') then
			next_state <= KEY4_4;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;
-- - - - - - - - - - - - - - - - - - - - - - -
   when KEY4_4 =>
      next_state <= KEY4_4;
      if (KEY(15) = '1') then
         next_state <= FINISH_ERR;
		elsif (KEY(1) = '1') then
			next_state <= KEY5_1;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;
-- - - - - - - - - - - - - - - - - - - - - - -
   when KEY5_1 =>
      next_state <= KEY5_1;
      if (KEY(15) = '1') then
         next_state <= FINISH_ERR;
		elsif (KEY(1) = '1') then
			next_state <= KEY6_1;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;
-- - - - - - - - - - - - - - - - - - - - - - -
   when KEY6_1 =>
      next_state <= KEY6_1;
      if (KEY(15) = '1') then
         next_state <= FINISH_ERR;
		elsif (KEY(6) = '1') then
			next_state <= KEY7_6;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;
-- - - - - - - - - - - - - - - - - - - - - - -
   when KEY7_6 =>
      next_state <= KEY7_6;
      if (KEY(15) = '1') then
         next_state <= FINISH_ERR;
		elsif (KEY(1) = '1') then
			next_state <= KEY8_1;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;
-- - - - - - - - - - - - - - - - - - - - - - -
   when KEY8_1 =>
      next_state <= KEY8_1;
      if (KEY(15) = '1') then
         next_state <= FINISH_ERR;
		elsif (KEY(2) = '1') then
			next_state <= KEY9_2;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;
-- - - - - - - - - - - - - - - - - - - - - - -
   when KEY9_2 =>
      next_state <= KEY9_2;
      if (KEY(15) = '1') then
         next_state <= FINISH_ERR;
		elsif (KEY(6) = '1') then
			next_state <= KEY10_6;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;	
-- - - - - - - - - - - - - - - - - - - - - - -
   when KEY10_6 =>
      next_state <= KEY10_6;
      if (KEY(15) = '1') then
         next_state <= FINISH_ERR;
		elsif (KEY(4) = '1') then
			next_state <= KEY11_4;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;	
-- - - - - - - - - - - - - - - - - - - - - - -
   when KEY11_4 =>
      next_state <= KEY11_4;
      if (KEY(15) = '1') then
         next_state <= FINISH_OK;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;		
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- - - - - - - - - - - - - - - - - - - - - - -
	   when ERROR =>
      next_state <= ERROR;
      if (KEY(15) = '1') then
         next_state <= FINISH_ERR;
		elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
		end if;	
	-- - - - - - - - - - - - - - - - - - - - - - -
	   when FINISH_ERR =>
      next_state <= FINISH_ERR;
      if (CNT_OF = '1') then
         next_state <= FINISH;
		end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	   when FINISH_OK =>
      next_state <= FINISH_OK;
      if (CNT_OF = '1') then
         next_state <= FINISH;
		end if;
		 -- - - - - - - - - - - - - - - - - - - - - - -
		when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= START;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -	
   when others =>
      next_state <= START;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is      
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH_OK =>
		FSM_MX_MEM     <= '1';
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
		-- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH_ERR =>
		FSM_MX_MEM     <= '0';
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   end case;
end process output_logic;

end architecture behavioral;

