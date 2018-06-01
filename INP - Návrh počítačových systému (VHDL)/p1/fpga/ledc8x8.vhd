library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
port ( -- Sem doplnte popis rozhrani obvodu.
	SMCLK: in std_logic;
	RESET: in std_logic;
	ROW: out std_logic_vector(7 downto 0);
	LED: out std_logic_vector(0 to 7)
);
end ledc8x8;

architecture main of ledc8x8 is

    -- Sem doplnte definice vnitrnich signalu.
    signal ce: std_logic := '0';
    signal counter: std_logic_vector(22 downto 0);
    signal switch: std_logic := '0';
    signal row_select: std_logic_vector(7 downto 0) := "00000001";
    signal led_select: std_logic_vector(0 to 7) := "00000000";
begin

    ctrl_cnt: process (SMCLK, RESET)
    begin
    	if RESET = '1'
		then
			counter <= "00000000000000000000000";
		elsif rising_edge(SMCLK) then
			if counter(7 downto 0) = "11111111" then
				ce <= '1';
			else
				ce <= '0';
			end if;
			switch <= counter(22);
			counter <= counter + 1;
		end if;
    end process;

    -- prepinani radku
    row_decoder: process(row_select,RESET,SMCLK)
	 begin
    	if RESET = '1' then
			row_select <= "10000000";
			ROW <= "10000000";
		elsif rising_edge(SMCLK) AND ce = '1' then 
			case row_select is
				when "10000000" => row_select <= "01000000";
				when "01000000" => row_select <= "00100000";
				when "00100000" => row_select <= "00010000";
				when "00010000" => row_select <= "00001000";
				when "00001000" => row_select <= "00000100";
				when "00000100" => row_select <= "00000010";
				when "00000010" => row_select <= "00000001";
				when "00000001" => row_select <= "10000000";
				when others => null;
			end case;
		end if;

		ROW <= row_select;
    end process;

    --prepinani led podle switch
    led_decoder: process(row_select,SMCLK,RESET,led_select,switch)
	 begin
	 	if RESET = '1' then
	 		led_select <= "00000000";
    	elsif rising_edge(SMCLK) then
    		--P
    		if switch = '1' then
	    		case row_select is
					when "00000001" => led_select <= "11000011";
					when "00000010" => led_select <= "11011101";
					when "00000100" => led_select <= "11011101";
					when "00001000" => led_select <= "11011101";
					when "00010000" => led_select <= "11000011";
					when "00100000" => led_select <= "11011111";
					when "01000000" => led_select <= "11011111";
					when "10000000" => led_select <= "11011111";
					when others => null;
				end case;
	    		--S
			else
	    		case row_select is
					when "00000001" => led_select <= "11000011";
					when "00000010" => led_select <= "10111101";
					when "00000100" => led_select <= "10111111";
					when "00001000" => led_select <= "11001111";
					when "00010000" => led_select <= "11110011";
					when "00100000" => led_select <= "11111101";
					when "01000000" => led_select <= "10111101";
					when "10000000" => led_select <= "11000011";
					when others => null;
				end case;
			end if;
    	end if ;    
		LED <= led_select(0 to 7);		
	end process;

end main;
